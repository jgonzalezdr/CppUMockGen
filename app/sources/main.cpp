#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <clang-c/Index.h>
#include <cxxopts.hpp>
#include "Function.hpp"
#include "Method.hpp"
#include "Config.hpp"
#include "ClangHelper.hpp"
#include "ConsoleColorizer.hpp"

static ConsoleColorizer cerrColorizer( ConsoleColorizer::ConsoleType::STD_ERROR );

const std::set<std::string> cppExtensions = { "hpp", "hxx", "hh" };

struct ParseData
{
    const Config& config;
    std::ostream &output;
};

bool parseFile( const std::string &input, const Config &config, bool interpretAsCpp,
                const std::vector<std::string> &includePaths, std::ostream &output )
{
    CXIndex index = clang_createIndex( 0, 0 );

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
                                                       CXTranslationUnit_SkipFunctionBodies );
    if( tu == nullptr )
    {
        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
        exit(-1);
    }

    unsigned int numDiags = clang_getNumDiagnostics(tu);
    unsigned int numErrors = 0;
    if( numDiags > 0 )
    {
        for( unsigned int i = 0; i < numDiags; i++ )
        {
            CXDiagnostic diag = clang_getDiagnostic( tu, i );

            CXDiagnosticSeverity diagSeverity = clang_getDiagnosticSeverity( diag );

            switch( diagSeverity )
            {
                case CXDiagnosticSeverity::CXDiagnostic_Fatal:
                case CXDiagnosticSeverity::CXDiagnostic_Error:
                    numErrors++;
                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
                    std::cerr << "PARSE ERROR: ";
                    break;

                case CXDiagnosticSeverity::CXDiagnostic_Warning:
                    cerrColorizer.SetColor( ConsoleColorizer::Color::YELLOW );
                    std::cerr << "PARSE WARNING: ";
                    break;

                default:
                    break;
            }

            cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );

            std::cerr << clang_formatDiagnostic( diag, clang_defaultDiagnosticDisplayOptions() ) << std::endl;

            clang_disposeDiagnostic( diag );
        }
    }

    if( numErrors == 0 )
    {
        ParseData parseData = { config, output };

        CXCursor tuCursor = clang_getTranslationUnitCursor(tu);
        clang_visitChildren(
            tuCursor,
            []( CXCursor cursor, CXCursor parent, CXClientData clientData )
            {
                ParseData *parseData = (ParseData*) clientData;
                if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) != 0 )
                {
                    CXCursorKind cursorKind = clang_getCursorKind( cursor );
                    if( cursorKind == CXCursor_FunctionDecl )
                    {
                        Function function( cursor, parseData->config );
                        if( function.IsMockable() )
                        {
                            parseData->output << function.GenerateMock() << std::endl;
                        }
                        return CXChildVisit_Continue;
                    }
                    else if( cursorKind == CXCursor_CXXMethod )
                    {
                        Method method( cursor, parseData->config );
                        if( method.IsMockable() )
                        {
                            parseData->output << method.GenerateMock() << std::endl;
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
            (CXClientData) &parseData );
    }

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return (numErrors == 0);
}

int main( int argc, char* argv[] )
{
    int returnCode = 0;

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
            std::cout << options.help();
            exit(0);
        }

        if( !options.count( "input" ) )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::string inputFileName = options["input"].as<std::string>();

        std::ofstream outputFile;
        if( options.count( "output" ) )
        {
            outputFile.open( options["output"].as<std::string>() );
            if( ! outputFile.is_open() )
            {
                throw std::runtime_error( "Output file could not be opened." );
            }
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

        std::ostringstream output;

        output << "/* This file has been auto-generated by CppUTestMock. DO NOT EDIT IT!!! */" << std::endl;
        output << std::endl;
        output << "#include <CppUTestExt/MockSupport.h>" << std::endl;
        if( !interpretAsCpp )
        {
            output << "extern \"C\" {" << std::endl;
        }
        output << "#include \"" <<  inputFileName << "\"" << std::endl;
        if( !interpretAsCpp )
        {
            output << "}" << std::endl;
        }
        output << std::endl;

        if( parseFile( inputFileName, config, interpretAsCpp, options["include-path"].as<std::vector<std::string>>(), output ) )
        {
            if( options.count( "output" ) )
            {
                outputFile << output.str();
            }
            else
            {
                std::cout << output.str();
            }
        }
        else
        {
            returnCode = 2;
            throw std::runtime_error( "Mock could not be generated due to errors parsing the input file." );
        }
    }
    catch(std::exception &e)
    {
        cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
        std::cerr << "ERROR: ";
        cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
        std::cerr << e.what();
        if( !returnCode )
        {
            returnCode = 1;
        }
    }

    return returnCode;
}
