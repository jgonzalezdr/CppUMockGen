#ifndef CPPUMOCKGEN_FILEHELPER_HPP_
#define CPPUMOCKGEN_FILEHELPER_HPP_

#include <string>

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

std::string GetFilenameFromPath( const std::string& filepath );

std::string RemoveFilenameExtension( const std::string &filepath );

bool IsDirPath( const std::string &path );

#endif // header guard
