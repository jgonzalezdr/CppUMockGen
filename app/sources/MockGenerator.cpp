#include "MockGenerator.hpp"

#include <iostream>
#include <fstream>
#include <clang-c/Index.h>

#include "Function.hpp"
#include "Method.hpp"
#include "ClangHelper.hpp"
#include "ConsoleColorizer.hpp"
#include "VersionInfo.h"

struct ParseData
{
    const Config& config;
    std::ostream &output;
};

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

static std::string GetFilenameFromPath( const std::string& filepath )
{
    size_t sepPos = filepath.rfind( PATH_SEPARATOR );
    if( sepPos == std::string::npos )
    {
        return filepath;
    }
    else
    {
        return filepath.substr( sepPos + 1 );
    }
}

void GenerateMock( CXTranslationUnit tu, const Config &config, std::ostream &output )
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

bool GenerateMock( const std::string &inputFilename, const Config &config, bool interpretAsCpp,
                   const std::vector<std::string> &includePaths, std::ostream &output, std::ostream &error )
{
    CXIndex index = clang_createIndex( 0, 0 );

    std::vector<const char*> clangOpts;
    if( interpretAsCpp )
    {
        clangOpts.push_back( "-xc++" );
    }

    std::vector<std::string> includePathOptions;

    for( const std::string &includePath : includePaths )
    {
        // Option strings must be stored in a vector, otherwise C strings would not be valid
        includePathOptions.push_back( "-I" + includePath );
        clangOpts.push_back( includePathOptions.back().c_str() );
    }

    CXTranslationUnit tu;
    CXErrorCode tuError = clang_parseTranslationUnit2( index, inputFilename.c_str(),
                                                       clangOpts.data(), (int) clangOpts.size(),
                                                       nullptr, 0,
                                                       CXTranslationUnit_SkipFunctionBodies,
                                                       &tu );
    if( tuError != CXError_Success )
    {
        cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
        error << "INPUT ERROR: ";
        cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );

        // Check if file exists
        std::ifstream inputFile( inputFilename.c_str() );
        if( !inputFile.good() )
        {
            error << "Input file '" << inputFilename.c_str() << "' does not exist" << std::endl;
        }
// LCOV_EXCL_START
        else
        {
            error << "Unable to parse input file (Error code = " << tuError << ")" << std::endl;
        }
// LCOV_EXCL_STOP

        clang_disposeIndex( index );
        return false;
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
                    error << "PARSE ERROR: ";
                    break;

                case CXDiagnosticSeverity::CXDiagnostic_Warning:
                    cerrColorizer.SetColor( ConsoleColorizer::Color::YELLOW );
                    error << "PARSE WARNING: ";
                    break;

// LCOV_EXCL_START
                default:
                    break;
// LCOV_EXCL_STOP
            }

            cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
            error << clang_formatDiagnostic( diag, clang_defaultDiagnosticDisplayOptions() ) << std::endl;

            clang_disposeDiagnostic( diag );
        }
    }

    if( numErrors == 0 )
    {
        output << "/*" << std::endl;
        output << " * This file has been auto-generated by CppUTestMock v" PRODUCT_VERSION_MAJOR_MINOR_PATCH_STR "." << std::endl;
        output << " *" << std::endl;
        output << " * Contents will NOT be preserved if it is regenerated!!!" << std::endl;
        output << " */" << std::endl;
        output << std::endl;
        if( !interpretAsCpp )
        {
            output << "extern \"C\" {" << std::endl;
        }
        output << "#include \"" <<  GetFilenameFromPath( inputFilename ) << "\"" << std::endl;
        if( !interpretAsCpp )
        {
            output << "}" << std::endl;
        }
        output << std::endl;
        output << "#include <CppUTestExt/MockSupport.h>" << std::endl;
        output << std::endl;

        GenerateMock( tu, config, output );
    }

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return (numErrors == 0);
}
