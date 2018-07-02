/**
 * @file
 * @brief      Header of file handling helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_FILEHELPER_HPP_
#define CPPUMOCKGEN_FILEHELPER_HPP_

#include <string>

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

std::string GetFilenameFromPath( const std::string& filepath ) noexcept;

std::string RemoveFilenameExtension( const std::string &filepath ) noexcept;

bool IsDirPath( const std::string &path ) noexcept;

#endif // header guard
