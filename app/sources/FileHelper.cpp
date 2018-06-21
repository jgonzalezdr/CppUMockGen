#include "FileHelper.hpp"

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
    return ( !path.empty() && ( path.back() == PATH_SEPARATOR ) );
}
