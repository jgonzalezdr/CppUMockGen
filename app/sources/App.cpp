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

    return ret;
}

int App::Execute( int argc, const char* argv[] ) noexcept
{
    int returnCode = 0;

    cxxopts::Options options( PRODUCT_NAME, PRODUCT_FILE_DESCRIPTION );

    options.add_options()
        ( "i,input", "Input file", cxxopts::value<std::string>(), "<input>" )
        ( "m,mock-output", "Mock output path", cxxopts::value<std::string>()->implicit_value( "" ), "<mock-output>" )
        ( "e,expect-output", "Expectation output path", cxxopts::value<std::string>()->implicit_value( "" ), "<expect-output>" )
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
            m_cout << options.help();
            return 0;
        }

        if( options.count("version") )
        {
            m_cout << PRODUCT_NAME " v" PRODUCT_VERSION_STR;
            return 0;
        }

        if( !options.count( "input" ) )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::filesystem::path inputFilepath = options["input"].as<std::string>();

        if( ( options.count( "mock-output" ) + options.count( "expect-output" ) ) == 0 )
        {
            throw std::runtime_error( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified." );
        }

        std::filesystem::path mockOutputFilepath;
        std::ofstream mockOutputFile;
        if( options.count( "mock-output" ) )
        {
            mockOutputFilepath = options["mock-output"].as<std::string>();
            if( mockOutputFilepath != "@" )
            {
                if( mockOutputFilepath.empty() || IsDirPath(mockOutputFilepath) )
                {
                    ConvertToDirPath( mockOutputFilepath );
                    mockOutputFilepath += inputFilepath.filename().stem().generic_string() + "_mock.cpp";
                }
                mockOutputFile.open( mockOutputFilepath );
                if( !mockOutputFile.is_open() )
                {
                    std::string errorMsg = "Mock output file '" + mockOutputFilepath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
            }
        }

        std::filesystem::path expectHeaderOutputFilepath;
        std::filesystem::path expectImplOutputFilepath;
        std::ofstream expectHeaderOutputFile;
        std::ofstream expectImplOutputFile;
        if( options.count( "expect-output" ) )
        {
            expectHeaderOutputFilepath = options["expect-output"].as<std::string>();
            if( expectHeaderOutputFilepath != "@" )
            {
                if( expectHeaderOutputFilepath.empty() || IsDirPath(expectHeaderOutputFilepath) )
                {
                    ConvertToDirPath( expectHeaderOutputFilepath );
                    std::string baseFilename = inputFilepath.stem().generic_string();
                    expectImplOutputFilepath = expectHeaderOutputFilepath;
                    expectImplOutputFilepath += baseFilename + "_expect.cpp";
                    expectHeaderOutputFilepath += baseFilename + "_expect.hpp";
                }
                else
                {
                    std::string baseFilename = expectHeaderOutputFilepath.stem().generic_string();
                    expectHeaderOutputFilepath = expectHeaderOutputFilepath.parent_path();
                    expectImplOutputFilepath = expectHeaderOutputFilepath;
                    expectImplOutputFilepath /= baseFilename + ".cpp";
                    expectHeaderOutputFilepath /= baseFilename + ".hpp";
                }

                expectHeaderOutputFile.open( expectHeaderOutputFilepath );
                if( !expectHeaderOutputFile.is_open() )
                {
                    std::string errorMsg = "Expectation header output file '" + expectHeaderOutputFilepath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }

                expectImplOutputFile.open( expectImplOutputFilepath );
                if( !expectImplOutputFile.is_open() )
                {
                    // LCOV_EXCL_START
                    std::string errorMsg = "Expectation implementation output file '" + expectImplOutputFilepath.generic_string() + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                    // LCOV_EXCL_STOP
                }
            }
        }

        bool interpretAsCpp = options[ "cpp" ].as<bool>();
        if( !interpretAsCpp )
        {
            std::string fileExtension = inputFilepath.extension().generic_string();

            interpretAsCpp = ( cppExtensions.count( fileExtension ) > 0 );
        }

        Config config( options["underlying-typedef"].as<bool>(),
                       options["type-override"].as<std::vector<std::string>>() );

        std::string genOpts = GetGenerationOptions( options );

        Parser parser;

        if( parser.Parse( inputFilepath, config, interpretAsCpp, options["std"].as<std::string>(), options["include-path"].as<std::vector<std::string>>(), m_cerr ) )
        {
            if( !mockOutputFilepath.empty() )
            {
                std::ostringstream output;
                parser.GenerateMock( genOpts, output );

                if( mockOutputFile.is_open() )
                {
                    mockOutputFile << output.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Mock generated into '" << mockOutputFilepath.generic_string() << "'" << std::endl;
                }
                else
                {
                    m_cout << output.str();
                }
            }

            if( !expectHeaderOutputFilepath.empty() )
            {
                std::ostringstream headerOutput;
                std::ostringstream implOutput;
                parser.GenerateExpectationHeader( genOpts, headerOutput );
                parser.GenerateExpectationImpl( genOpts, expectHeaderOutputFilepath, implOutput );

                if( expectHeaderOutputFile.is_open() )
                {
                    expectHeaderOutputFile << headerOutput.str();
                    expectImplOutputFile << implOutput.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Expectations generated into '" << expectHeaderOutputFilepath.generic_string() << 
                              "' and '" << expectImplOutputFilepath.generic_string() << "'" << std::endl;
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
            std::string errorMsg = "Output could not be generated due to errors parsing the input file '" + inputFilepath.generic_string() + "'.";
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

