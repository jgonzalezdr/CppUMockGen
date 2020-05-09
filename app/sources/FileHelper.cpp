/**
 * @file
 * @brief      Implementation of file handling helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "FileHelper.hpp"

#include <filesystem>

bool IsDirPath( const std::filesystem::path &path ) noexcept
{
    return ( !path.empty() && 
             ( !path.has_filename() ||
               std::filesystem::is_directory( path ) ) );
}

void ConvertToDirPath( std::filesystem::path &path ) noexcept
{
    if( !path.empty() && path.has_filename() )
    {
        path /= "";
    }
}
