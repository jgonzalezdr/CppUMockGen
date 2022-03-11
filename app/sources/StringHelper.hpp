/**
 * @file
 * @brief      Header of string helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_STRINGHELPER_HPP_
#define CPPUMOCKGEN_STRINGHELPER_HPP_

#include <string>

std::string TrimString( const std::string &str );

size_t StringCount( const std::string &str, char c );

std::string ToLower( const std::string &str );

#endif // header guard
