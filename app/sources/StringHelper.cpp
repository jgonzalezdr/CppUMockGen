/**
 * @file
 * @brief      Implementation of string helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "StringHelper.hpp"

#include <algorithm>

std::string TrimString( const std::string &str )
{
    size_t first = str.find_first_not_of( ' ' );
    if( first == std::string::npos )
    {
        return str;
    }
    size_t last = str.find_last_not_of( ' ' );
    return str.substr( first, ( last - first + 1 ) );
}

size_t StringCount( const std::string &str, char c )
{
    size_t count = 0;
    size_t pos = 0;
    while( pos != std::string::npos )
    {
        pos = str.find( c, pos );
        if( pos != std::string::npos )
        {
            pos++;
            count++;
        }
    }

    return count;
}

std::string ToLower( const std::string &str )
{
    std::string ret = str;
    std::transform( ret.begin(), ret.end(), ret.begin(), [](unsigned char c){ return std::tolower(c); } );
    return ret;
}
