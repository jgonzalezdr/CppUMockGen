/**
 * @file
 * @brief      Implementation of file handling helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "FileHelper.hpp"

#include <filesystem>

std::string GetFilenameFromPath( const std::string& filepath ) noexcept
{
    size_t sepPos = filepath.rfind( PATH_SEPARATOR );
    if( sepPos == std::string::npos )
    {
        return filepath;
    }
    else
    {
        return filepath.substr( sepPos + 1 );
    }
}

std::string RemoveFilenameExtension( const std::string &filepath ) noexcept
{
    size_t sepPos = filepath.find(".");
    if( sepPos == std::string::npos )
    {
        return filepath;
    }
    else
    {
        return filepath.substr( 0, sepPos );
    }
}

bool IsDirPath( const std::string& path ) noexcept
{
    return ( !path.empty() && 
             ( ( path.back() == PATH_SEPARATOR ) ||
               std::filesystem::is_directory( path ) ) );
}

void ConvertToDirPath( std::string& path ) noexcept
{
    if( !path.empty() && ( path.back() != PATH_SEPARATOR ) )
    {
        path += PATH_SEPARATOR;
    }
}
