#include <iostream>
#include <fstream>
#include <set>
#include <clang-c/Index.h>
#include <cxxopts.hpp>
#include "Function.hpp"
#include "Method.hpp"
#include "Config.hpp"

const std::set<std::string> cppExtensions = { "hpp", "hxx", "hh" };

std::ostream* output;

void parseFile( const std::string &input, const Config &config, bool interpretAsCpp,
                const std::vector<std::string> &includePaths )
{
    CXIndex index = clang_createIndex( 0, 1 );

    std::vector<const char*> clangOpts;
    if( interpretAsCpp )
    {
        clangOpts.push_back( "-xc++" );
    }

    std::vector<std::string> includePathOpts;

    for( const std::string &includePath : includePaths )
    {
        includePathOpts.push_back( "-I" + includePath );
        clangOpts.push_back( includePathOpts.back().c_str() );
    }

    CXTranslationUnit tu = clang_parseTranslationUnit( index, input.c_str(),
                                                       clangOpts.data(), clangOpts.size(),
                                                       nullptr, 0,
                                                       CXTranslationUnit_None );
    if( tu == nullptr )
    {
        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
        exit(-1);
    }

    CXCursor tuCursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(
        tuCursor,
        []( CXCursor cursor, CXCursor parent, CXClientData client_data )
        {
            if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) != 0 )
            {
                CXCursorKind cursorKind = clang_getCursorKind( cursor );
                if( cursorKind == CXCursor_FunctionDecl )
                {
                    Function function( cursor, *(Config *)client_data );
                    if( function.IsMockable() )
                    {
                        *output << function.GenerateMock() << std::endl;
                    }
                    return CXChildVisit_Continue;
                }
                else if( cursorKind == CXCursor_CXXMethod )
                {
                    Method method( cursor, *(Config *)client_data );
                    if( method.IsMockable() )
                    {
                        *output << method.GenerateMock() << std::endl;
                    }
                    return CXChildVisit_Continue;
                }
                else
                {
                    //            std::cout << "Cursor '" << clang_getCursorSpelling( cursor ) << "' of kind '"
                    //                << clang_getCursorKindSpelling( clang_getCursorKind( cursor ) ) << "'\n";
                    return CXChildVisit_Recurse;
                }
            }
            else
            {
                return CXChildVisit_Continue;
            }
        },
        (CXClientData) &config );

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );
}

int main( int argc, char* argv[] )
{
    cxxopts::Options options("CppUMockGen", "Mock generator for CppUTest");

    options.add_options()
        ( "i,input", "Input file", cxxopts::value<std::string>(), "<input>" )
        ( "o,output", "Output file", cxxopts::value<std::string>(), "<output>" )
        ( "x,cpp", "Force interpretation of the input file as C++", cxxopts::value<bool>(), "<force-cpp>" )
        ( "u,underlying-typedef", "Use underlying typedef type", cxxopts::value<bool>(), "[<underlying-typedef>]" )
        ( "I,include-path", "Include path", cxxopts::value<std::vector<std::string>>(), "<path>" )
        ( "h,help", "Print help" );

    options.positional_help( "[<input>] [<output>]" );
    options.parse_positional( std::vector<std::string> { "input", "output" } );

    try
    {
        options.parse( argc, argv );

        if( options.count("help") )
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        if( !options.count( "input" ) )
        {
            std::cerr << "No input file specified" << std::endl;
            exit(1);
        }

        std::string inputFileName = options["input"].as<std::string>();

        std::ofstream outputFile;

        if( !options.count( "output" ) )
        {
            output = &std::cout;
        }
        else
        {
            outputFile.open( options["output"].as<std::string>() );
            if( ! outputFile.is_open() )
            {
                std::cerr << "Error opening output file" << std::endl;
                exit(1);
            }
            output = &outputFile;
        }

        bool interpretAsCpp = false;
        if( options["cpp"].as<bool>() )
        {
            interpretAsCpp = true;
        }
        else
        {
            size_t extensionIndex = inputFileName.find_last_of(".");
            if( extensionIndex != std::string::npos )
            {
                std::string fileExtension = inputFileName.substr( extensionIndex + 1 );
                interpretAsCpp = ( cppExtensions.count( fileExtension ) > 0 );
            }
        }

        Config config( options["underlying-typedef"].as<bool>() );

        *output << "// This file has been auto-generated by CppUTestMock. DO NOT EDIT IT!!!" << std::endl;
        *output << std::endl;
        *output << "#include <CppUTestExt/MockSupport.h>" << std::endl;
        if( !interpretAsCpp )
        {
            *output << "extern \"C\" {" << std::endl;
        }
        *output << "#include \"" <<  inputFileName << "\"" << std::endl;
        if( !interpretAsCpp )
        {
            *output << "}" << std::endl;
        }
        *output << std::endl;

        parseFile( inputFileName, config, interpretAsCpp, options["include-path"].as<std::vector<std::string>>() );

        return 0;
    }
    catch(cxxopts::OptionParseException &e)
    {
        std::cerr << e.what();
        return 1;
    }
}
