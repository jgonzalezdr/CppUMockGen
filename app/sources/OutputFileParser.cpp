/**
 * @file
 * @brief      Implementation of OutputFileParser class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2022 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "OutputFileParser.hpp"

#include <fstream>
#include <regex>

static const std::regex g_userCodeBeginRegex( "(?:\\/\\/|\\/\\*)\\s*" USER_CODE_BEGIN );
static const std::regex g_userCodeEndRegex( "(?:\\/\\/|\\/\\*)\\s*" USER_CODE_END );
static const std::regex g_generationOptionsRegex( GENERATION_OPTIONS_LABEL "(.*)" );

OutputFileParser::OutputFileParser()
{
}

const std::string& OutputFileParser::GetUserCode() const
{
    return m_userCode;
}

const std::string& OutputFileParser::GetGenerationOptions() const
{
    return m_generationOptions;
}

void OutputFileParser::Parse( const std::filesystem::path &filepath )
{
    bool captureUserCode = false;
    std::string line;

    std::ifstream outputFile( filepath );
    if( outputFile.is_open() )
    {
        while( getline( outputFile, line ) )
        {
            ParseLine( line, captureUserCode );
        }
        outputFile.close();
    }

    if( captureUserCode )
    {
        // User code is not properly delimited => ignore it.
        m_userCode.clear();
    }
}

void OutputFileParser::ParseLine( const std::string &line, bool &captureUserCode )
{
    std::smatch regexMatch;

    if( std::regex_search( line, g_userCodeBeginRegex ) )
    {
        captureUserCode = true;
    }
    else if( std::regex_search( line, g_userCodeEndRegex ) )
    {
        captureUserCode = false;
    }
    else if( std::regex_search( line, regexMatch, g_generationOptionsRegex ) )
    {
        m_generationOptions = regexMatch[1].str();
    }
    else if( captureUserCode )
    {
        m_userCode += line + "\n";
    }
}
