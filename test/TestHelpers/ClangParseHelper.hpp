/**
 * @file
 * @brief      Header of Clang parsing helper class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CLANG_PARSE_HELPER_HPP_
#define CLANG_PARSE_HELPER_HPP_

#include <clang-c/Index.h>
#include <string>
#include <functional>

/**
 * Static class containing functions used to parse C/C++ code files.
 */
class ClangParseHelper
{
public:
    /**
     * Parses C/C++ code and calls a callback function for each parsed C/C++ function.
     * @param code [in] Code to be parsed
     * @param callback [in] Function to be called for each parsed C/C++ function
     */
    static void ParseHeader( const std::string &code, const std::function<void( CXCursor )> &callback );

private:
    ClangParseHelper() {}; // Disables instantation of static class
};

#endif // header guard
