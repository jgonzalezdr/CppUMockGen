/**
 * @file
 * @brief      Generic implemention of CppUTest tests main function
 * @project    Generic
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2014-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"

int main( int argc, char* argv[] )
{
    MockSupportPlugin mockPlugin;

    TestRegistry::getCurrentRegistry()->installPlugin( &mockPlugin );

    return CommandLineTestRunner::RunAllTests( argc, argv );
}
