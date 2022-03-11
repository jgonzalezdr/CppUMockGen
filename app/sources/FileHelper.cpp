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
#include <set>

#include "StringHelper.hpp"

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

const std::set<std::string> cppHeaderExtensions = { ".hpp", ".hxx", ".hh" };
const std::set<std::string> cppImplExtensions = { ".cpp", ".cxx", ".cc" };

static bool IsCppHeaderExtension( const std::string &extension ) noexcept
{
    return ( cppHeaderExtensions.count( extension ) > 0 );
}

static bool IsCppImplExtension( const std::string &extension ) noexcept
{
    return ( cppImplExtensions.count( extension ) > 0 );
}

bool HasCppExtension( const std::filesystem::path &path ) noexcept
{
    auto extension = ToLower( path.extension().generic_string() );
    return IsCppImplExtension( extension ) || IsCppHeaderExtension( extension );
}

bool HasCppImplExtension( const std::filesystem::path &path ) noexcept
{
    auto extension = ToLower( path.extension().generic_string() );
    return IsCppImplExtension( extension );
}

bool HasCppHeaderExtension( const std::filesystem::path &path ) noexcept
{
    auto extension = ToLower( path.extension().generic_string() );
    return IsCppHeaderExtension( extension );
}
