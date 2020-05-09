/**
 * @file
 * @brief      Implementation of ConfigFile class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "ConfigFile.hpp"

#include <fstream>
#include <set>
#include <filesystem>

#include "cxxopts.hpp"

#include "ConsoleColorizer.hpp"
#include "VersionInfo.h"

void AddConfigFileOptions( cxxopts::Options &options )
{
    options.add_options()
        ( "I,include-path", "Include path", cxxopts::value<std::vector<std::string>>(), "<path>" )
        ( "p,param-override", "Override parameter type", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "t,type-override", "Override generic type", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "f,config-file", "Config file", cxxopts::value<std::vector<std::string>>(), "<file-path>" );
}

static void ProcessConfigLine( const std::string &line, std::vector<std::string> &params, const std::filesystem::path &configFilepath, unsigned int lineNum )
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
                        errorMsg << "In configuration file '" << configFilepath.generic_string() << "':" << lineNum << ":" << curPos << " ending quote was not found.";
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

static void ProcessConfigFile( const std::filesystem::path &configFilepath, cxxopts::Options &options )
{
    std::vector<std::string> params;
    params.push_back( "" );

    unsigned int lineNum = 1;
    std::string line;

    std::ifstream configFile( configFilepath );
    if( configFile.is_open() )
    {
        while( getline( configFile, line ) )
        {
            ProcessConfigLine( line, params, configFilepath, lineNum );
            lineNum++;
        }
        configFile.close();
    }
    else
    {
        std::string errorMsg = "Configuration file '" + configFilepath.generic_string() + "' could not be opened.";
        throw std::runtime_error( errorMsg );
    }

    if( params.size() > 0 )
    {
        int argc = (int) params.size();
        char **argv = new char *[ argc ];

        for( int i = 0; i < argc; i++ )
        {
            argv[ i ] = (char *) params[ i ].c_str();
        }

        options.parse( argc, argv );

        delete[] argv;
    }
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

void ProcessConfigFiles( cxxopts::Options &cmdLineOptions )
{
    // Create config file options parser

    cxxopts::Options configFileOptions( PRODUCT_NAME, PRODUCT_FILE_DESCRIPTION );
    AddConfigFileOptions( configFileOptions );

    // Move config file options from command line to config file parser

    {
        std::vector<std::string> &cmdLineVector = 
            const_cast<std::vector<std::string> &>( cmdLineOptions[ "config-file" ].as<std::vector<std::string>>() );
        std::vector<std::string> &cfgFileVector = 
            const_cast<std::vector<std::string> &>( configFileOptions[ "config-file" ].as<std::vector<std::string>>() );

        cfgFileVector.insert( cfgFileVector.end(),
                              std::make_move_iterator( cmdLineVector.begin() ),
                              std::make_move_iterator( cmdLineVector.end() ) );
    }

    // Process config files

    std::set<std::filesystem::path> processedConfigFiles;
    ProcessConfigFiles( configFileOptions, processedConfigFiles, "" );

    // Move options from config files to command line parser

    {
        std::vector<std::string> &cmdLineVector =
            const_cast<std::vector<std::string> &>( cmdLineOptions[ "include-path" ].as<std::vector<std::string>>() );
        std::vector<std::string> &cfgFileVector =
            const_cast<std::vector<std::string> &>( configFileOptions[ "include-path" ].as<std::vector<std::string>>() );

        cmdLineVector.insert( cmdLineVector.end(),
                              std::make_move_iterator( cfgFileVector.begin() ),
                              std::make_move_iterator( cfgFileVector.end() ) );
    }
    {
        std::vector<std::string> &cmdLineVector = 
            const_cast<std::vector<std::string> &>( cmdLineOptions[ "param-override" ].as<std::vector<std::string>>() );
        std::vector<std::string> &cfgFileVector = 
            const_cast<std::vector<std::string> &>( configFileOptions[ "param-override" ].as<std::vector<std::string>>() );

        cmdLineVector.insert( cmdLineVector.end(),
                              std::make_move_iterator( cfgFileVector.begin() ),
                              std::make_move_iterator( cfgFileVector.end() ) );
    }
    {
        std::vector<std::string> &cmdLineVector = 
            const_cast<std::vector<std::string> &>( cmdLineOptions[ "type-override" ].as<std::vector<std::string>>() );
        std::vector<std::string> &cfgFileVector = 
            const_cast<std::vector<std::string> &>( configFileOptions[ "type-override" ].as<std::vector<std::string>>() );

        cmdLineVector.insert( cmdLineVector.end(),
                              std::make_move_iterator( cfgFileVector.begin() ),
                              std::make_move_iterator( cfgFileVector.end() ) );
    }
}