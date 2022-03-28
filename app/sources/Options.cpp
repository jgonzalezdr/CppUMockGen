/**
 * @file
 * @brief      Implementation of Options class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2022 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "Options.hpp"

#include <fstream>
#include <vector>
#include <set>
#include <filesystem>

#include "VersionInfo.h"

Options::Options()
: m_options( PRODUCT_NAME, PRODUCT_FILE_DESCRIPTION )
{
    m_options.add_options()
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
        ( "r,regen", "Use previous generate options from output file", cxxopts::value<bool>(), "<regenerate>" )
        ( "v,version", "Print version" )
        ( "h,help", "Print help" );

    m_options.positional_help( "<input>" );
    m_options.parse_positional( std::vector<std::string> { "input" } );
}

static void ProcessConfigLine( const std::string &line, std::vector<std::string> &params )
{
    size_t curPos = 0;
    size_t len = line.length();

    while( curPos < len )
    {
        if( line[ curPos ] == ' ' )
        {
            curPos++;
        }
        else
        {
            size_t delimPos;

            if( line[ curPos ] == '\"' )
            {
                // Parameters with spaces can be passed using quotes, which can be escaped using a backslash
                curPos++;
                delimPos = curPos;
                bool foundDelim = false;
                while( !foundDelim )
                {
                    delimPos = line.find_first_of( '\"', delimPos );
                    if( delimPos != std::string::npos )
                    {
                        if( line[ delimPos - 1 ] == '\\' )
                        {
                            // The found quote is skipped since it's escaped
                            delimPos++;
                        }
                        else
                        {
                            foundDelim = true;
                        }
                    }
                    else
                    {
                        std::ostringstream errorMsg;
                        errorMsg << curPos << ": Ending quote was not found.";
                        throw std::runtime_error( errorMsg.str() );
                    }
                }
            }
            else
            {
                delimPos = line.find_first_of( ' ', curPos );
            }

            std::string param;
            if( delimPos != std::string::npos )
            {
                param = line.substr( curPos, delimPos - curPos );
                curPos = delimPos + 1;
            }
            else
            {
                param = line.substr( curPos );
                curPos = len;
            }

            params.push_back( param );
        }
    }
}

static void ProcessParams( const std::vector<std::string> &params, cxxopts::Options &options )
{
    if( params.size() > 1 )
    {
        int argc = (int) params.size();
        char *argv[ argc ];
        char **argvp = argv;

        for( int i = 0; i < argc; i++ )
        {
            argv[ i ] = (char *) params[ i ].c_str();
        }

        options.parse( argc, argvp );
    }
}

static void ProcessConfigFile( const std::filesystem::path &configFilepath, cxxopts::Options &options )
{
    std::vector<std::string> params;
    params.push_back( "" );

    unsigned int lineNum = 1;
    std::string line;

    std::ifstream configFile( configFilepath );
    if( configFile.is_open() )
    {
        try
        {
            while( getline( configFile, line ) )
            {
                ProcessConfigLine( line, params );
                lineNum++;
            }
        }
        catch( const std::exception& e )
        {
            std::ostringstream errorMsg;
            errorMsg << "In configuration file '" << configFilepath.generic_string() << "':" << lineNum << ":" << e.what();
            throw std::runtime_error( errorMsg.str() );
        }
        
        configFile.close();
    }
    else
    {
        std::string errorMsg = "Configuration file '" + configFilepath.generic_string() + "' could not be opened.";
        throw std::runtime_error( errorMsg );
    }

    ProcessParams( params, options );
}

static void ProcessConfigFiles( cxxopts::Options &options, std::set<std::filesystem::path> &processedConfigFiles,
                                const std::filesystem::path &path )
{
    std::vector<std::string> configFiles = options[ "config-file" ].as<std::vector<std::string>>();
    const_cast<std::vector<std::string> &>( options[ "config-file" ].as<std::vector<std::string>>() ).clear();
    for( const std::string &configFilename : configFiles )
    {
        std::filesystem::path configFilepath = configFilename;
        if( configFilepath.is_relative() )
        {
            configFilepath = path / configFilepath;
        }
        configFilepath = configFilepath.lexically_normal();

        std::filesystem::path configFilepathAbsolute = std::filesystem::absolute( configFilepath );

        //printf( "Processing config file '%s'\n", configFilepath.generic_string().c_str() );

        // Only process each file once to avoid infinite recursion
        if( processedConfigFiles.count( configFilepathAbsolute ) == 0 )
        {
            processedConfigFiles.insert( configFilepathAbsolute );

            ProcessConfigFile( configFilepath, options );

            // Process config files included by the processed config file
            ProcessConfigFiles( options, processedConfigFiles, configFilepath.parent_path() );
        }
    }
}

void Options::Parse( int argc, const char* argv[] )
{
    m_options.parse( argc, const_cast<char**&>(argv) );

    std::set<std::filesystem::path> processedConfigFiles;
    ProcessConfigFiles( m_options, processedConfigFiles, "" );
}

void Options::Parse( const std::string &optionsStr )
{
    std::vector<std::string> params;
    params.push_back( "" );

    try
    {
        ProcessConfigLine( optionsStr, params );
    }
    catch( const std::exception& e )
    {
        std::ostringstream errorMsg;
        errorMsg << "In options '" << optionsStr << "':" << e.what();
        throw std::runtime_error( errorMsg.str() );
    }

    ProcessParams( params, m_options );
}

bool Options::IsHelpRequested() const
{
    return ( m_options.count("help") > 0 );
}

std::string Options::GetHelpText() const
{
    return m_options.help(); // LCOV_EXCL__BR_LINE: False positive
}

bool Options::IsVersionRequested() const
{
    return ( m_options.count("version") > 0 );
}

std::string Options::GetInputPath() const
{
    return m_options["input"].as<std::string>();
}

std::string Options::GetBaseDirectory() const
{
    return m_options["base-directory"].as<std::string>();
}

bool Options::IsMockRequested() const
{
    return ( m_options.count( "mock-output" ) > 0 );
}

std::string Options::GetMockOutputPath() const
{
    return m_options["mock-output"].as<std::string>();
}

bool Options::IsExpectationsRequested() const
{
    return ( m_options.count( "expect-output" ) > 0 );
}

std::string Options::GetExpectationsOutputPath() const
{
    return m_options["expect-output"].as<std::string>();
}

bool Options::IsRegenerationRequested() const
{
    return m_options["regen"].as<bool>();
}

bool Options::InterpretAsCpp() const
{
    return m_options["cpp"].as<bool>();
}

std::string Options::GetLanguageStandard() const
{
    return m_options["std"].as<std::string>();
}

bool Options::UseUnderlyingTypedef() const
{
    return m_options["underlying-typedef"].as<bool>();
}

std::vector<std::string> Options::GetTypeOverrides() const
{
    return m_options["type-override"].as<std::vector<std::string>>();
}

std::vector<std::string> Options::GetIncludePaths() const
{
    return m_options["include-path"].as<std::vector<std::string>>();
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

std::string Options::GetGenerationOptions() const
{
    std::string ret;

    if( InterpretAsCpp() )
    {
        ret += "-x ";
    }

    std::string languageStandard = GetLanguageStandard();
    if( !languageStandard.empty() )
    {
        ret += "-s " + QuotifyOption( languageStandard ) + " ";
    }

    if( UseUnderlyingTypedef() )
    {
        ret += "-u ";
    }

    for( auto typeOverride : GetTypeOverrides() )
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
