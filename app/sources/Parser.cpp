#include "Parser.hpp"

#include <iostream>
#include <fstream>
#include <clang-c/Index.h>

#include "Config.hpp"
#include "Function.hpp"
#include "Method.hpp"
#include "ClangHelper.hpp"
#include "ConsoleColorizer.hpp"
#include "FileHelper.hpp"
#include "VersionInfo.h"

struct ParseData
{
    const Config& config;
    std::vector<std::unique_ptr<const Function>> &functions;
};

void Parse( CXTranslationUnit tu, const Config &config, std::vector<std::unique_ptr<const Function>> &functions )
{
    ParseData parseData = { config, functions };

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
                    Function *function = new Function;
                    if( function->Parse( cursor, parseData->config ) )
                    {
                        parseData->functions.push_back( std::unique_ptr<const Function>(function) );
                    }
                    else
                    {
                        delete function;
                    }
                    return CXChildVisit_Continue;
                }
                else if( cursorKind == CXCursor_CXXMethod )
                {
                    Method *method = new Method;
                    if( method->Parse( cursor, parseData->config ) )
                    {
                        parseData->functions.push_back( std::unique_ptr<const Function>(method) );
                    }
                    else
                    {
                        delete method;
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

bool Parser::Parse( const std::string &inputFilepath, const Config &config, bool interpretAsCpp,
                    const std::vector<std::string> &includePaths, std::ostream &error )
{
    m_inputFilepath = inputFilepath;
    m_interpretAsCpp = interpretAsCpp;

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
    // Note: Use of CXTranslationUnit_SkipFunctionBodies is not allowed, otherwise libclang
    // will not detect properly methods defined inline (which must not be mocked).
    CXErrorCode tuError = clang_parseTranslationUnit2( index, inputFilepath.c_str(),
                                                       clangOpts.data(), (int) clangOpts.size(),
                                                       nullptr, 0,
                                                       CXTranslationUnit_None,
                                                       &tu );
    if( tuError != CXError_Success )
    {
        cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
        error << "INPUT ERROR: ";
        cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );

        // Check if file exists
        std::ifstream inputFile( inputFilepath.c_str() );
        if( !inputFile.good() )
        {
            error << "Input file '" << inputFilepath.c_str() << "' does not exist." << std::endl;
        }
// LCOV_EXCL_START
        else
        {
            error << "Unable to parse input file (Error code = " << tuError << ")." << std::endl;
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
        ::Parse( tu, config, m_functions );

        if( m_functions.size() == 0 )
        {
            cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
            error << "INPUT ERROR: ";
            cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
            error << "The input file does not contain any mockable function." << std::endl;
        }
    }

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return (numErrors == 0) && (m_functions.size() > 0);
}

void Parser::GenerateMock( const std::string &genOpts, std::ostream &output ) const noexcept
{
    GenerateFileHeading( genOpts, output );

    if( !m_interpretAsCpp )
    {
        output << "extern \"C\" {" << std::endl;
    }
    output << "#include \"" <<  GetFilenameFromPath( m_inputFilepath ) << "\"" << std::endl;
    if( !m_interpretAsCpp )
    {
        output << "}" << std::endl;
    }
    output << std::endl;

    output << "#include <CppUTestExt/MockSupport.h>" << std::endl;
    output << std::endl;

    for( const std::unique_ptr<const Function> &function : m_functions )
    {
        output << function->GenerateMock() << std::endl;
    }
}

void Parser::GenerateExpectationHeader( const std::string &genOpts, std::ostream &output ) const noexcept
{
    GenerateFileHeading( genOpts, output );

    output << "#include <CppUMockGen.h>" << std::endl;
    output << std::endl;

    if( !m_interpretAsCpp )
    {
        output << "extern \"C\" {" << std::endl;
    }
    output << "#include \"" <<  GetFilenameFromPath( m_inputFilepath ) << "\"" << std::endl;
    if( !m_interpretAsCpp )
    {
        output << "}" << std::endl;
    }
    output << std::endl;

    output << "#include <CppUTestExt/MockSupport.h>" << std::endl;
    output << std::endl;

    for( const std::unique_ptr<const Function> &function : m_functions )
    {
        output << function->GenerateExpectation(true) << std::endl;
    }
}

void Parser::GenerateExpectationImpl( const std::string &genOpts, const std::string &headerFilepath, std::ostream &output ) const noexcept
{
    GenerateFileHeading( genOpts, output );

    output << "#include \"" <<  GetFilenameFromPath( headerFilepath ) << "\"" << std::endl;
    output << std::endl;

    for( const std::unique_ptr<const Function> &function : m_functions )
    {
        output << function->GenerateExpectation(false) << std::endl;
    }
}

void Parser::GenerateFileHeading( const std::string &genOpts, std::ostream &output ) const noexcept
{
    output << "/*" << std::endl;
    output << " * This file has been auto-generated by CppUTestMock v" PRODUCT_VERSION_STR "." << std::endl;
    output << " *" << std::endl;
    output << " * Contents will NOT be preserved if it is regenerated!!!" << std::endl;
    if( !genOpts.empty() )
    {
        output << " *" << std::endl;
        output << " * Generation options: " << genOpts << std::endl;
    }
    output << " */" << std::endl;
    output << std::endl;
}
