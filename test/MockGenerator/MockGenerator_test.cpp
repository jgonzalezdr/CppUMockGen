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
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <direct.h>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "MockGenerator.hpp"
#include "FileHelper.hpp"

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

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

static const std::string tempDirPath = std::string(std::getenv("TEMP"));
static const std::string tempFilename = "CppUMockGen_MockGenerator.h";
static const std::string tempFilePath = tempDirPath + PATH_SEPARATOR + tempFilename;
static const std::string nonexistingFilePath = tempDirPath + PATH_SEPARATOR + "CppUMockGen_MockGenerator_NotExisting.h";

TEST_GROUP( MockGenerator )
{
    std::ofstream tempFile;
    std::string initialDir;

    TEST_SETUP()
    {
        initialDir = getcwd( NULL, 0 );
    }

    TEST_TEARDOWN()
    {
        chdir( initialDir.c_str() );
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
   std::ostringstream error;
   const char* testMock = "###MOCK###";

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), "", output, error );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( testMock, output.str().c_str() );
   STRCMP_CONTAINS( "extern \"C\"", output.str().c_str() );
   CHECK_EQUAL( 0, error.tellp() )

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
    std::ostringstream error;
   const char* testMock = "###MOCK###";

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};";
    SetupTempFile( testHeader );

    mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
    mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

    // Exercise
    std::vector<std::string> results;
    bool result = GenerateMock( tempFilePath, *config, true, std::vector<std::string> {".."}, "", output, error );

    // Verify
    CHECK_EQUAL( true, result );
    STRCMP_CONTAINS( testMock, output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() )

    // Cleanup
}

/*
 * Check that mocking a method works as expected.
 */
TEST( MockGenerator, MultipleFunctionsAndMethods )
{
    // Prepare
    Config* config = GetMockConfig();
    std::ostringstream output;
    std::ostringstream error;
   const char* testMock[] = { "### MOCK 1 ###\n", "### MOCK 2 ###\n", "### MOCK 3 ###\n", "### MOCK 4 ###\n" };

    SimpleString testHeader =
            "void function1(int a);\n"
            "int function2();\n"
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "    double method2(int*);\n"
            "};";
    SetupTempFile( testHeader );

    mock().expectNCalls(2, "Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
    mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock[0]);
    mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock[1]);
    mock().expectNCalls(2, "Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
    mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock[2]);
    mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock[3]);

    // Exercise
    std::vector<std::string> results;
    bool result = GenerateMock( tempFilePath, *config, true, std::vector<std::string> {".."}, "", output, error );

    // Verify
    CHECK_EQUAL( true, result );
    STRCMP_CONTAINS( testMock[0], output.str().c_str() );
    STRCMP_CONTAINS( testMock[1], output.str().c_str() );
    STRCMP_CONTAINS( testMock[2], output.str().c_str() );
    STRCMP_CONTAINS( testMock[3], output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() )

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
   std::ostringstream error;

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(false);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), "", output, error );

   // Verify
   CHECK_EQUAL( true, result );
   CHECK_EQUAL( 0, error.tellp() )

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
   std::ostringstream error;

   SimpleString testHeader =
           "foo function1(int a);";
   SetupTempFile( testHeader );

   mock().expectNCalls(2, "ConsoleColorizer::SetColor").ignoreOtherParameters();

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), "", output, error );

   // Verify
   CHECK_EQUAL( false, result );
   CHECK_EQUAL( 0, output.tellp() )
   STRCMP_CONTAINS( "PARSE ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_MockGenerator.h:1:1: error: unknown type name 'foo'", error.str().c_str() );

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
   std::ostringstream error;
   const char* testMock = "###MOCK###";

   SimpleString testHeader =
           "#warning test\n"
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectNCalls(2, "ConsoleColorizer::SetColor").ignoreOtherParameters();
   mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), "", output, error );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( testMock, output.str().c_str() );
   STRCMP_CONTAINS( "PARSE WARNING:", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_MockGenerator.h:1:2: warning: test [-W#warnings]", error.str().c_str() );

   // Cleanup
}

/*
 * Check that an error is issued when the input file does not exist.
 */
TEST( MockGenerator, NonExistingInputFile )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;
   std::ostringstream error;

   std::remove( nonexistingFilePath.c_str() );

   mock().expectNCalls(2, "ConsoleColorizer::SetColor").ignoreOtherParameters();

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( nonexistingFilePath, *config, false, std::vector<std::string>(), "", output, error );

   // Verify
   CHECK_EQUAL( false, result );
   CHECK_EQUAL( 0, output.tellp() )
   STRCMP_CONTAINS( "INPUT ERROR: Input file '", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_MockGenerator_NotExisting.h' does not exist", error.str().c_str() );

   // Cleanup
}

/*
 * Check that an error is issued when the input file does not exist.
 */
TEST( MockGenerator, PathWithoutDirectories )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;
   std::ostringstream error;

   const char* testMock = "###MOCK###";

   SimpleString testHeader =
           "class class1 {\n"
           "public:\n"
           "    void method1();\n"
           "};";
   SetupTempFile( testHeader );

   chdir( tempDirPath.c_str() );

   mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue(testMock);

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilename, *config, true, std::vector<std::string> {".."}, "", output, error );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( testMock, output.str().c_str() );
   CHECK_EQUAL( 0, error.tellp() )

   // Cleanup
}

/*
 * Check that regeneration options are printed properly.
 */
TEST( MockGenerator, WithRegenOpts )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream output;
   std::ostringstream error;
   const char* testRegenOpts = "####REGEN_OPTS######";

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   mock().expectOneCall("Function::Parse").withConstPointerParameter("config", config).ignoreOtherParameters().andReturnValue(true);
   mock().expectOneCall("Function::GenerateMock").andReturnValue("");

   // Exercise
   std::vector<std::string> results;
   bool result = GenerateMock( tempFilePath, *config, false, std::vector<std::string>(), testRegenOpts, output, error );

   // Verify
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( StringFromFormat( "Generation options: %s", testRegenOpts ).asCharString(), output.str().c_str() );
   CHECK_EQUAL( 0, error.tellp() )

   // Cleanup
}

