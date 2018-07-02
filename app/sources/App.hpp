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
     * @param cout [out] Stream where standard output will be printed
     * @param cerr [out] Stream where error output will be printed
     */
    App( std::ostream &cout, std::ostream &cerr ) noexcept;

    /**
     * Executes the application.
     *
     * @param argc [in] Number of arguments
     * @param argv [in] Array of pointer to the arguments
     * @return Exit code
     */
    int Execute( int argc, const char* argv[] ) noexcept;

private:
    void PrintError( const char *msg ) noexcept;

    std::ostream &m_cout;
    std::ostream &m_cerr;
};

#endif // header guard
