/**
 * @file
 * @brief      Implementation of App class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "App.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <filesystem>

#include "Options.hpp"
#include "Config.hpp"
#include "Parser.hpp"
#include "ConsoleColorizer.hpp"
#include "FileHelper.hpp"
#include "OutputFileParser.hpp"

#include "VersionInfo.h"

#define MOCK_FILE_SUFFIX "_mock"
#define EXPECTATION_FILE_SUFFIX "_expect"
#define IMPL_FILE_EXTENSION ".cpp"
#define HEADER_FILE_EXTENSION ".hpp"

App::App( std::ostream &cout, std::ostream &cerr ) noexcept
: m_cout(cout), m_cerr(cerr)
{
}

void App::PrintError( const char *msg ) noexcept
{
    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
    m_cerr << "ERROR: ";
    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
    m_cerr << msg << std::endl;
}

static std::filesystem::path CombinePath( const std::filesystem::path &path, const char* fileSuffix )
{
    std::filesystem::path ret;

    if( IsDirPath( path ) || ( path == "@" ) )
    {
        ret = path;
    }
    else
    {
        if( HasCppExtension( path ) )
        {
            ret = path.parent_path() / path.stem();
        }
        else
        {
            ret = path;
        }
        ret += fileSuffix;
    }

    return ret;
}

int App::Execute( int argc, const char* argv[] ) noexcept
{
    int returnCode = 0;

    Options options;

    try
    {
        options.Parse( argc, argv );

        if( options.IsHelpRequested() )
        {
            m_cout << options.GetHelpText();
            return 0;
        }

        if( options.IsVersionRequested() )
        {
            m_cout << PRODUCT_NAME " v" PRODUCT_VERSION_STR << std::endl;
            return 0;
        }

        std::filesystem::path inputFilePath = options.GetInputPath();
        if( inputFilePath.empty() )
        {
            throw std::runtime_error( "No input file specified." );
        }

        bool generateMock = options.IsMockRequested();
        bool generateExpectation = options.IsExpectationsRequested();
        if( !( generateMock || generateExpectation ) )
        {
            throw std::runtime_error( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified." );
        }

        std::filesystem::path baseDirPath = options.GetBaseDirectory();
        if( !baseDirPath.empty() && !IsDirPath( baseDirPath ) )
        {
            std::string errorMsg = "Base directory path '" + baseDirPath.generic_string() + "' is not an existing directory.";
            throw std::runtime_error( errorMsg );
        }

        bool regenerate = options.IsRegenerationRequested();
        std::string regenerateOptions;

        std::filesystem::path mockOutputFilePath = options.GetMockOutputPath();
        std::filesystem::path expectationHeaderOutputFilePath = options.GetExpectationsOutputPath();
        if( generateMock && generateExpectation )
        {
            // Use common mock/expection generation path if one is passed explicitly but not the other.
            if( mockOutputFilePath.empty() && !expectationHeaderOutputFilePath.empty() )
            {
                mockOutputFilePath = CombinePath( expectationHeaderOutputFilePath, MOCK_FILE_SUFFIX );
            }
            else if( expectationHeaderOutputFilePath.empty() && !mockOutputFilePath.empty() )
            {
                expectationHeaderOutputFilePath = CombinePath( mockOutputFilePath, EXPECTATION_FILE_SUFFIX );
            }
        }

        std::ofstream mockOutputStream;
        std::string mockUserCode;
        if( generateMock )
        {
            if( mockOutputFilePath != "@" )
            {
                if( mockOutputFilePath.empty() || IsDirPath(mockOutputFilePath) )
                {
                    ConvertToDirPath( mockOutputFilePath );
                    mockOutputFilePath /= inputFilePath.filename().stem();
                    mockOutputFilePath += MOCK_FILE_SUFFIX IMPL_FILE_EXTENSION;
                }
                else if( !HasCppImplExtension( mockOutputFilePath ) )
                {
                    mockOutputFilePath += IMPL_FILE_EXTENSION;
                }

                OutputFileParser outputFileParser;
                outputFileParser.Parse( mockOutputFilePath );
                mockUserCode = outputFileParser.GetUserCode();
                if( regenerate )
                {
                    regenerateOptions = outputFileParser.GetGenerationOptions();
                }

                mockOutputStream.open( mockOutputFilePath );
                if( !mockOutputStream.is_open() )
                {
                    std::string errorMsg = "Mock output file '" + mockOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
            }
        }

        std::filesystem::path expectationImplOutputFilePath;
        std::ofstream expectationHeaderOutputStream;
        std::ofstream expectationImplOutputStream;
        if( generateExpectation )
        {
            if( expectationHeaderOutputFilePath != "@" )
            {
                if( expectationHeaderOutputFilePath.empty() || IsDirPath(expectationHeaderOutputFilePath) )
                {
                    ConvertToDirPath( expectationHeaderOutputFilePath );
                    expectationHeaderOutputFilePath /= inputFilePath.stem();
                    expectationImplOutputFilePath = expectationHeaderOutputFilePath;
                    expectationHeaderOutputFilePath += EXPECTATION_FILE_SUFFIX HEADER_FILE_EXTENSION;
                    expectationImplOutputFilePath += EXPECTATION_FILE_SUFFIX IMPL_FILE_EXTENSION;
                }
                else
                {
                    if( HasCppExtension( expectationHeaderOutputFilePath ) )
                    {
                        expectationHeaderOutputFilePath = expectationHeaderOutputFilePath.parent_path() / expectationHeaderOutputFilePath.stem();
                    }

                    expectationImplOutputFilePath = expectationHeaderOutputFilePath;
                    expectationHeaderOutputFilePath += HEADER_FILE_EXTENSION;
                    expectationImplOutputFilePath += IMPL_FILE_EXTENSION;
                }

                expectationHeaderOutputStream.open( expectationHeaderOutputFilePath );
                if( !expectationHeaderOutputStream.is_open() )
                {
                    std::string errorMsg = "Expectation header output file '" + expectationHeaderOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }

                if( regenerate && regenerateOptions.empty() )
                {
                    OutputFileParser headerFileParser;
                    headerFileParser.Parse( expectationHeaderOutputFilePath );
                    regenerateOptions = headerFileParser.GetGenerationOptions();

                    if( regenerateOptions.empty() )
                    {
                        OutputFileParser implFileParser;
                        implFileParser.Parse( expectationImplOutputFilePath );
                        regenerateOptions = implFileParser.GetGenerationOptions();
                    }
                }

                expectationImplOutputStream.open( expectationImplOutputFilePath );
                // LCOV_EXCL_START: Defensive
                if( !expectationImplOutputStream.is_open() )
                {
                    std::string errorMsg = "Expectation implementation output file '" + expectationImplOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
                // LCOV_EXCL_STOP
            }
        }

        if( regenerate )
        {
            options.Parse( regenerateOptions );
        }

        bool isCppHeader = HasCppHeaderExtension( inputFilePath );

        Config config( options.InterpretAsCpp(),
                       options.GetLanguageStandard(),
                       options.UseUnderlyingTypedef(),
                       options.GetTypeOverrides() );

        std::string genOpts = options.GetGenerationOptions();

        Parser parser;

        if( parser.Parse( inputFilePath, config, isCppHeader, options.GetIncludePaths(), m_cerr ) )
        {
            if( generateMock )
            {
                std::filesystem::path mockBaseDirPath = baseDirPath;
                if( mockBaseDirPath.empty() )
                {
                    mockBaseDirPath = mockOutputFilePath.parent_path();
                }

                std::ostringstream output;
                parser.GenerateMock( genOpts, mockUserCode, mockBaseDirPath, output );

                if( mockOutputStream.is_open() )
                {
                    mockOutputStream << output.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Mock generated into '" << mockOutputFilePath.generic_string() << "'" << std::endl;
                }
                else
                {
                    m_cout << output.str();
                }
            }

            if( generateExpectation )
            {
                std::filesystem::path expectBaseDirPath = baseDirPath;
                if( expectBaseDirPath.empty() )
                {
                    expectBaseDirPath = expectationHeaderOutputFilePath.parent_path();
                }

                std::ostringstream headerOutput;
                std::ostringstream implOutput;
                parser.GenerateExpectationHeader( genOpts, expectBaseDirPath, headerOutput );
                parser.GenerateExpectationImpl( genOpts, expectationHeaderOutputFilePath, implOutput );

                if( expectationHeaderOutputStream.is_open() )
                {
                    expectationHeaderOutputStream << headerOutput.str();
                    expectationImplOutputStream << implOutput.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Expectations generated into '" << expectationHeaderOutputFilePath.generic_string() << 
                              "' and '" << expectationImplOutputFilePath.generic_string() << "'" << std::endl;
                }
                else
                {
                    m_cout << headerOutput.str();
                    m_cout << implOutput.str();
                }
            }
        }
        else
        {
            returnCode = 2;
            std::string errorMsg = "Output could not be generated due to errors parsing the input file '" + inputFilePath.generic_string() + "'.";
            throw std::runtime_error( errorMsg );
        }
    }
    catch(std::exception &e)
    {
        PrintError( e.what() );
        if( !returnCode )
        {
            returnCode = 1;
        }
    }

    return returnCode;
}

