/**
 * @file
 * @brief      unit tests for the "Method" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "MockGenerator.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

Config* GetMockConfig()
{
    return (Config*) (void*) 836487567;
}

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

static std::string tempFilePath = std::string(std::getenv("TEMP")) + "\\CppUMockGen_MockGenerator.h";

TEST_GROUP( MockGenerator )
{
    std::ofstream tempFile;

    TEST_TEARDOWN()
    {
        std::remove( tempFilePath.c_str() );
    }

    void SetupTempFile( const SimpleString& contents )
    {
        tempFile.open( tempFilePath );
        tempFile << contents.asCharString();
        tempFile.close();
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that mocking a function works as expected.
 */
TEST( MockGenerator, MockedFunction )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;
   const char* testMock = "###MOCK###";

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
   mock().expectOneCall("Function::IsMockable").andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), output );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( testMock, output.str().c_str() );
   STRCMP_CONTAINS( "extern \"C\"", output.str().c_str() );

   // Cleanup
}

/*
 * Check that mocking a method works as expected.
 */
TEST( MockGenerator, MockedMethod )
{
    // Prepare
    Config* config = GetMockConfig();
    std::ostringstream output;
    const char* testMock = "###MOCK###";

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};";
    SetupTempFile( testHeader );

    mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
    mock().expectOneCall("Method::Method").withConstPointerParameter("config", config).ignoreOtherParameters();
    mock().expectOneCall("Method::IsMockable").andReturnValue(true);
    mock().expectOneCall("Method::GenerateMock").andReturnValue(testMock);

    // Exercise
    std::vector<std::string> results;
    bool result = GenerateMock( tempFilePath, *config, true, std::vector<std::string> {".."}, output );

    // Verify
    CHECK_EQUAL( true, result );
    STRCMP_CONTAINS( testMock, output.str().c_str() );

    // Cleanup
}

/*
 * Check that mocking a non-mockable function works as expected.
 */
TEST( MockGenerator, NonMockable )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
   mock().expectOneCall("Function::IsMockable").andReturnValue(false);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), output );

   // Verify
   CHECK_EQUAL( true, result );

   // Cleanup
}

/*
 * Check that a syntax error is aborts mock generation.
 */
TEST( MockGenerator, SyntaxError )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;

   SimpleString testHeader =
           "foo function1(int a);";
   SetupTempFile( testHeader );

   //mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
   //mock().expectOneCall("Function::IsMockable").andReturnValue(false);
   mock().expectNCalls(2, "ConsoleColorizer::SetColor").ignoreOtherParameters();

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), output );

   // Verify
   CHECK_EQUAL( false, result );

   // Cleanup
}

/*
 * Check that a warning is handled without aborting mock generation.
 */
TEST( MockGenerator, Warning )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;
   const char* testMock = "###MOCK###";

   SimpleString testHeader =
           "#warning test\n"
           "void function1(int a);";
   SetupTempFile( testHeader );

   //mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
   //mock().expectOneCall("Function::IsMockable").andReturnValue(false);
   mock().expectNCalls(2, "ConsoleColorizer::SetColor").ignoreOtherParameters();
   mock().expectOneCall("Function::Function").withConstPointerParameter("config", config).ignoreOtherParameters();
   mock().expectOneCall("Function::IsMockable").andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), output );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( testMock, output.str().c_str() );

   // Cleanup
}

