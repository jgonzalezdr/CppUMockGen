/**
 * @file
 * @brief      Header of file handling helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_FILEHELPER_HPP_
#define CPPUMOCKGEN_FILEHELPER_HPP_

#include <string>
#include <filesystem>

bool IsDirPath( const std::filesystem::path &path ) noexcept;

void ConvertToDirPath( std::filesystem::path &path ) noexcept;

bool HasCppExtension( const std::filesystem::path &path ) noexcept;
bool HasCppImplExtension( const std::filesystem::path &path ) noexcept;
bool HasCppHeaderExtension( const std::filesystem::path &path ) noexcept;

#endif // header guard
