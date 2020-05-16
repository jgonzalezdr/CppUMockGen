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

#include "clang-c/Index.h"

int main( int argc, char* argv[] )
{
#ifdef AVOID_LIBCLANG_MEMLEAK
    // Some libClang versions allocate global data on first call, which is detectect
    // as a memory leak if this happens during a test
    CXIndex avoidMemLeakIndex = clang_createIndex( 0, 0 );
    CXTranslationUnit avoidMemLeakTu;
    clang_parseTranslationUnit2( avoidMemLeakIndex, "", nullptr, 0, nullptr, 0,
                                 CXTranslationUnit_None, &avoidMemLeakTu );
    clang_disposeIndex( avoidMemLeakIndex );
#endif

    MockSupportPlugin mockPlugin;

    TestRegistry::getCurrentRegistry()->installPlugin( &mockPlugin );
    //MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();

    return CommandLineTestRunner::RunAllTests( argc, argv );
}
