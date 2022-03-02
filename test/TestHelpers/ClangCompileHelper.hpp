/**
 * @file
 * @brief      Header of Clang compiling helper class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CLANG_COMPILE_HELPER_HPP_
#define CLANG_COMPILE_HELPER_HPP_

#include <clang-c/Index.h>
#include <string>
#include <functional>

/**
 * Static class containing functions used to check that generated code compiles properly.
 */
class ClangCompileHelper
{
public:
    /**
     * Checks that a generated mock compiles properly.
     *
     * @param inputHeader [in] Input header used to generate the mock
     * @param testedSource [in] Generated mock source
     * @return @c true if the compilation was successful, @c false otherwise
     */
    static bool CheckMockCompilation( const std::string &inputHeader, const std::string &testedSource );

    /**
     * Checks that a generated expectation helper compiles properly.
     *
     * @param inputHeader [in] Input header used to generate the mock
     * @param testedHeader [in] Generated expectation helper header
     * @param testedSource [in] Generated expectation helper source
     * @return @c true if the compilation was successful, @c false otherwise
     */
    static bool CheckExpectationCompilation( const std::string &inputHeader, 
                                             const std::string &testedHeader, const std::string &testedSource );

private:
    ClangCompileHelper() {}; // Disables instantation of static class
};

#endif // header guard
