/**
 * @file
 * @brief      Implementation of executable entry point
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "App.hpp"

#include <iostream>

int main( int argc, const char* argv[] )
{
    App app( std::cout, std::cerr );
    return app.Execute( argc, argv );
}
