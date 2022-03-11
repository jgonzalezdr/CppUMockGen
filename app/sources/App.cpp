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

#include <cxxopts.hpp>

#include "Parser.hpp"
#include "Config.hpp"
#include "ConsoleColorizer.hpp"
#include "FileHelper.hpp"
#include "ConfigFile.hpp"

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

static std::string QuotifyOption( const std::string &option ) noexcept
{
    if( option.find_first_of(" \t=&|,;^%@$!#*?(){}[]<>\\") != std::string::npos )
    {
        return "\"" + option + "\"";
    }
    else
    {
        return option;
    }
}

static std::string GetGenerationOptions( cxxopts::Options &options ) noexcept
{
    std::string ret;

    if( options["cpp"].as<bool>() )
    {
        ret += "-x ";
    }

    if( ( options["std"].count() > 0 ) && !options["std"].as<std::string>().empty() )
    {
        ret += "-s " + QuotifyOption( options["std"].as<std::string>() ) + " ";
    }

    if( options["underlying-typedef"].as<bool>() )
    {
        ret += "-u ";
    }

    for( auto typeOverride : options["type-override"].as<std::vector<std::string>>() )
    {
        ret += "-t " + QuotifyOption( typeOverride ) + " ";
    }

    // Delete the trailing whitespace
    if( !ret.empty() )
    {
        ret.pop_back();
    }

    return ret;
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

    cxxopts::Options options( PRODUCT_NAME, PRODUCT_FILE_DESCRIPTION );

    options.add_options()
        ( "i,input", "Input file path", cxxopts::value<std::string>(), "<input>" )
        ( "m,mock-output", "Mock output directory or file path", cxxopts::value<std::string>()->implicit_value( "" ), "<mock-output>" )
        ( "e,expect-output", "Expectation output directory or file path", cxxopts::value<std::string>()->implicit_value( "" ), "<expect-output>" )
        ( "x,cpp", "Force interpretation of the input file as C++", cxxopts::value<bool>(), "<force-cpp>" )
        ( "s,std", "Set language standard", cxxopts::value<std::string>(), "<standard>" )
        ( "u,underlying-typedef", "Use underlying typedef type", cxxopts::value<bool>(), "<underlying-typedef>" )
        ( "I,include-path", "Include path", cxxopts::value<std::vector<std::string>>(), "<path>" )
        ( "B,base-directory", "Base directory path", cxxopts::value<std::string>(), "<path>" )
        ( "t,type-override", "Type override", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "f,config-file", "Config file", cxxopts::value<std::vector<std::string>>(), "<file-path>" )
        ( "v,version", "Print version" )
        ( "h,help", "Print help" );

    options.positional_help( "<input>" );
    options.parse_positional( std::vector<std::string> { "input" } );

    try
    {
        options.parse( argc, const_cast<char**&>(argv) );

        ProcessConfigFiles( options );

        if( options.count("help") > 0 )
        {
            m_cout << options.help(); // LCOV_EXCL_BR_LINE: False positive
            return 0;
        }

        if( options.count("version") > 0  )
        {
            m_cout << PRODUCT_NAME " v" PRODUCT_VERSION_STR << std::endl;
            return 0;
        }

        if( options.count( "input" ) <= 0 )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::filesystem::path inputFilePath = options["input"].as<std::string>();

        if( ( options.count( "mock-output" ) + options.count( "expect-output" ) ) <= 0 )
        {
            throw std::runtime_error( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified." );
        }

        std::filesystem::path baseDirPath;
        if( options.count( "base-directory" ) > 0 )
        {
            baseDirPath = options["base-directory"].as<std::string>();
            if( !IsDirPath( baseDirPath ) )
            {
                std::string errorMsg = "Base directory path '" + baseDirPath.generic_string() + "' is not an existing directory.";
                throw std::runtime_error( errorMsg );
            }
        }

        bool generateMock = false;
        std::filesystem::path mockOutputFilePath;
        if( options.count( "mock-output" ) > 0 )
        {
            mockOutputFilePath = options["mock-output"].as<std::string>();
            generateMock = true;
        }

        bool generateExpectation = false;
        std::filesystem::path expectationHeaderOutputFilePath;
        if( options.count( "expect-output" ) > 0 )
        {
            expectationHeaderOutputFilePath = options["expect-output"].as<std::string>();
            generateExpectation = true;
        }

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

        bool interpretAsCpp = options[ "cpp" ].as<bool>();
        if( !interpretAsCpp )
        {
            interpretAsCpp = HasCppHeaderExtension( inputFilePath );
        }

        Config config( options["underlying-typedef"].as<bool>(),
                       options["type-override"].as<std::vector<std::string>>() );

        std::string genOpts = GetGenerationOptions( options );

        Parser parser;

        if( parser.Parse( inputFilePath, config, interpretAsCpp, options["std"].as<std::string>(), options["include-path"].as<std::vector<std::string>>(), m_cerr ) )
        {
            if( generateMock )
            {
                std::filesystem::path mockBaseDirPath = baseDirPath;
                if( mockBaseDirPath.empty() )
                {
                    mockBaseDirPath = mockOutputFilePath.parent_path();
                }

                std::ostringstream output;
                parser.GenerateMock( genOpts, mockBaseDirPath, output );

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

