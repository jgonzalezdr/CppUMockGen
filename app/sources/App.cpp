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

const std::set<std::string> cppExtensions = { ".hpp", ".hxx", ".hh" };

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
    if( option.find(' ') != std::string::npos )
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

        if( options.count("help") )
        {
            m_cout << options.help(); // LCOV_EXCL_BR_LINE: False positive
            return 0;
        }

        if( options.count("version") )
        {
            m_cout << PRODUCT_NAME " v" PRODUCT_VERSION_STR << std::endl;
            return 0;
        }

        if( !options.count( "input" ) )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::filesystem::path inputFilePath = options["input"].as<std::string>();

        if( ( options.count( "mock-output" ) + options.count( "expect-output" ) ) == 0 )
        {
            throw std::runtime_error( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified." );
        }

        std::filesystem::path mockOutputFilePath;
        std::ofstream mockOutputStream;
        if( options.count( "mock-output" ) )
        {
            mockOutputFilePath = options["mock-output"].as<std::string>();
            if( mockOutputFilePath != "@" )
            {
                if( mockOutputFilePath.empty() || IsDirPath(mockOutputFilePath) )
                {
                    ConvertToDirPath( mockOutputFilePath );
                    mockOutputFilePath += inputFilePath.filename().stem().generic_string() + "_mock.cpp";
                }
                mockOutputStream.open( mockOutputFilePath );
                if( !mockOutputStream.is_open() )
                {
                    std::string errorMsg = "Mock output file '" + mockOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
            }
        }

        std::filesystem::path expectHeaderOutputFilePath;
        std::filesystem::path expectImplOutputFilePath;
        std::ofstream expectHeaderOutputStream;
        std::ofstream expectImplOutputStream;
        if( options.count( "expect-output" ) )
        {
            expectHeaderOutputFilePath = options["expect-output"].as<std::string>();
            if( expectHeaderOutputFilePath != "@" )
            {
                if( expectHeaderOutputFilePath.empty() || IsDirPath(expectHeaderOutputFilePath) )
                {
                    ConvertToDirPath( expectHeaderOutputFilePath );
                    std::string baseFilename = inputFilePath.stem().generic_string();
                    expectImplOutputFilePath = expectHeaderOutputFilePath;
                    expectImplOutputFilePath += baseFilename + "_expect.cpp";
                    expectHeaderOutputFilePath += baseFilename + "_expect.hpp";
                }
                else
                {
                    std::string baseFilename = expectHeaderOutputFilePath.stem().generic_string();
                    expectHeaderOutputFilePath = expectHeaderOutputFilePath.parent_path();
                    expectImplOutputFilePath = expectHeaderOutputFilePath;
                    expectImplOutputFilePath /= baseFilename + ".cpp";
                    expectHeaderOutputFilePath /= baseFilename + ".hpp";
                }

                expectHeaderOutputStream.open( expectHeaderOutputFilePath );
                if( !expectHeaderOutputStream.is_open() )
                {
                    std::string errorMsg = "Expectation header output file '" + expectHeaderOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }

                expectImplOutputStream.open( expectImplOutputFilePath );
                // LCOV_EXCL_START: Defensive
                if( !expectImplOutputStream.is_open() )
                {
                    std::string errorMsg = "Expectation implementation output file '" + expectImplOutputFilePath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
                // LCOV_EXCL_STOP
            }
        }

        bool interpretAsCpp = options[ "cpp" ].as<bool>();
        if( !interpretAsCpp )
        {
            std::string fileExtension = inputFilePath.extension().generic_string();

            interpretAsCpp = ( cppExtensions.count( fileExtension ) > 0 );
        }

        Config config( options["underlying-typedef"].as<bool>(),
                       options["type-override"].as<std::vector<std::string>>() );

        std::string genOpts = GetGenerationOptions( options );

        Parser parser;

        if( parser.Parse( inputFilePath, config, interpretAsCpp, options["std"].as<std::string>(), options["include-path"].as<std::vector<std::string>>(), m_cerr ) )
        {
            if( !mockOutputFilePath.empty() )
            {
                std::ostringstream output;
                parser.GenerateMock( genOpts, output );

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

            if( !expectHeaderOutputFilePath.empty() )
            {
                std::ostringstream headerOutput;
                std::ostringstream implOutput;
                parser.GenerateExpectationHeader( genOpts, headerOutput );
                parser.GenerateExpectationImpl( genOpts, expectHeaderOutputFilePath, implOutput );

                if( expectHeaderOutputStream.is_open() )
                {
                    expectHeaderOutputStream << headerOutput.str();
                    expectImplOutputStream << implOutput.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Expectations generated into '" << expectHeaderOutputFilePath.generic_string() << 
                              "' and '" << expectImplOutputFilePath.generic_string() << "'" << std::endl;
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

