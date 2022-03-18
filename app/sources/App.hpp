/**
 * @file
 * @brief      Header of App class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_APP_HPP_
#define CPPUMOCKGEN_APP_HPP_

#include <ostream>

/**
 * Class that implements the application.
 */
class App
{
public:
    /**
     * Constructor.
     *
     * @param[out] cout Stream where standard output will be printed
     * @param[out] cerr Stream where error output will be printed
     */
    App( std::ostream &cout, std::ostream &cerr ) noexcept;

    /**
     * Executes the application.
     *
     * @param[in] argc Number of arguments
     * @param[in] argv Array of pointer to the arguments
     * @return Exit code
     */
    int Execute( int argc, const char* argv[] ) noexcept;

private:
    void PrintError( const char *msg ) noexcept;

    std::ostream &m_cout;
    std::ostream &m_cerr;
};

#endif // header guard
