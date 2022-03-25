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

OutputFileParser::OutputFileParser()
: m_captureUserCode( false )
{
}

const std::string& OutputFileParser::GetUserCode() const
{
    return m_userCode;
}

void OutputFileParser::Parse( const std::filesystem::path &filepath )
{
    std::string line;

    std::ifstream outputFile( filepath );
    if( outputFile.is_open() )
    {
        while( getline( outputFile, line ) )
        {
            ParseLine( line );
        }
        outputFile.close();
    }

    if( m_captureUserCode )
    {
        // User code is not properly delimited => ignore it.
        m_userCode.clear();
    }
}

void OutputFileParser::ParseLine( const std::string &line )
{
    if( std::regex_search( line, g_userCodeBeginRegex ) )
    {
        m_captureUserCode = true;
    }
    else if( std::regex_search( line, g_userCodeEndRegex ) )
    {
        m_captureUserCode = false;
    }
    else if( m_captureUserCode )
    {
        m_userCode += line + "\n";
    }
}
