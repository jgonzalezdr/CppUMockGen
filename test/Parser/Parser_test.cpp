/**
 * @file
 * @brief      Unit tests for the "Parser" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>
#include <functional>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "Parser.hpp"
#include "FileHelper.hpp"

#include "Function_expect.hpp"
#include "ConsoleColorizer_expect.hpp"

using CppUMockGen::IgnoreParameter;

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

static const std::filesystem::path tempDirPath = std::filesystem::temp_directory_path();
static const std::string tempFilename = "CppUMockGen_Parser.h";
static const std::string tempFilePath = (tempDirPath / tempFilename).generic_string();
static const std::string nonexistingFilePath = (tempDirPath / "CppUMockGen_Parser_NotExisting.h").generic_string();

TEST_GROUP( Parser )
{
    std::ofstream tempFile;
    std::filesystem::path initialDir;

    TEST_SETUP()
    {
        initialDir = std::filesystem::current_path();
    }

    TEST_TEARDOWN()
    {
        std::filesystem::current_path( initialDir );

        std::filesystem::remove( tempFilePath );
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
TEST( Parser, MockedFunction )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilePath, *config, false, "", std::vector<std::string>(), error );

   // Verify
   mock().checkExpectations();
   CHECK_EQUAL( true, result );
   CHECK_EQUAL( 0, error.tellp() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output1;
   const char* testMock = "###MOCK6768###";

   expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

   // Exercise
   parser.GenerateMock( "", output1 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testMock, output1.str().c_str() );
   STRCMP_CONTAINS( "extern \"C\"", output1.str().c_str() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output2;
   const char* testExpect1 = "###EXPECT3178###";

   expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

   // Exercise
   parser.GenerateExpectationHeader( "", output2 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
   STRCMP_CONTAINS( "extern \"C\"", output2.str().c_str() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output3;
   const char* testExpect2 = "###EXPECT3682###";

   expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

   // Exercise
   parser.GenerateExpectationImpl( "", "my_header.h", output3 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
   STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that mocking a method works as expected.
 */
TEST( Parser, MockedMethod )
{
    // Prepare
    Config* config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, true, "", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char* testMock = "###MOCK###";

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock, output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char* testExpect1 = "###EXPECT87828763###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1, output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char* testExpect2 = "###EXPECT87362###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that passing a language standard works as expected.
 */
TEST( Parser, MockedMethod_Cpp17 )
{
    // Prepare
    Config *config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
        "#include <filesystem>\n"
        "class class1 {\n"
        "public:\n"
        "    bool method1(std::filesystem::path &p);\n"
        "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, true, "c++17", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char *testMock = "###MOCK###";

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock, output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char *testExpect1 = "###EXPECT87828763###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1, output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char *testExpect2 = "###EXPECT87362###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that passing a C++ language standard implicitly set the language to C++.
 */
TEST( Parser, MockedMethod_Cpp14 )
{
    // Prepare
    Config *config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
        "class class1 {\n"
        "public:\n"
        "    bool method1();\n"
        "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, false, "c++14", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char *testMock = "###MOCK###";

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock, output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char *testExpect1 = "###EXPECT87828763###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1, output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char *testExpect2 = "###EXPECT87362###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that passing a GNU++ language standard implicitly set the language to C++.
 */
TEST( Parser, MockedMethod_Gnu98 )
{
    // Prepare
    Config *config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
        "class class1 {\n"
        "public:\n"
        "    bool method1();\n"
        "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, false, "gnu++98", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char *testMock = "###MOCK###";

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock, output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char *testExpect1 = "###EXPECT87828763###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1, output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char *testExpect2 = "###EXPECT87362###";

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that mocking a method works as expected.
 */
TEST( Parser, MultipleMockableFunctionsAndMethods )
{
    // Prepare
    Config* config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
            "void function1(int a);\n"
            "int function2();\n"
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "    double method2(int*);\n"
            "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( 4, IgnoreParameter::YES, IgnoreParameter::YES, config, true );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, true, "", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char* testMock[] = { "### MOCK 1 ###\n", "### MOCK 2 ###\n", "### MOCK 3 ###\n", "### MOCK 4 ###\n" };

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[0] );
    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[1] );
    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[2] );
    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[3] );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock[0], output1.str().c_str() );
    STRCMP_CONTAINS( testMock[1], output1.str().c_str() );
    STRCMP_CONTAINS( testMock[2], output1.str().c_str() );
    STRCMP_CONTAINS( testMock[3], output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char* testExpect1[] = { "### EXPECT H 1 ###\n", "### EXPECT H 2 ###\n", "### EXPECT H 3 ###\n", "### EXPECT H 4 ###\n" };

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[0] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[1] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[2] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[3] );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1[0], output2.str().c_str() );
    STRCMP_CONTAINS( testExpect1[1], output2.str().c_str() );
    STRCMP_CONTAINS( testExpect1[2], output2.str().c_str() );
    STRCMP_CONTAINS( testExpect1[3], output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char* testExpect2[] = { "### EXPECT I 1 ###\n", "### EXPECT I 2 ###\n", "### EXPECT I 3 ###\n", "### EXPECT I 4 ###\n" };

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[0] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[1] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[2] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[3] );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2[0], output3.str().c_str() );
    STRCMP_CONTAINS( testExpect2[1], output3.str().c_str() );
    STRCMP_CONTAINS( testExpect2[2], output3.str().c_str() );
    STRCMP_CONTAINS( testExpect2[3], output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that mocking a non-mockable function works as expected.
 */
TEST( Parser, FunctionNonMockable )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   SimpleString testHeader =
           "void function1(int a);";
   SetupTempFile( testHeader );

   expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
   expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilePath, *config, false, "", std::vector<std::string>(), error );

   // Verify
   CHECK_EQUAL( false, result );
   STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

   // Cleanup
}

/*
 * Check that mocking a non-mockable method works as expected.
 */
TEST( Parser, MethodNonMockable )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   SimpleString testHeader =
           "class class1 {\n"
           "public:\n"
           "    void method1();\n"
           "};";
   SetupTempFile( testHeader );

   expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
   expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilePath, *config, true, "", std::vector<std::string>(), error );

   // Verify
   CHECK_EQUAL( false, result );
   STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

   // Cleanup
}

/*
 * Check that mocking a method works as expected.
 */
TEST( Parser, MixedMockableNonMockableFunctionsAndMethods )
{
    // Prepare
    Config* config = GetMockConfig();
    std::ostringstream error;

    SimpleString testHeader =
            "void function1(int a);\n"
            "int function2();\n"
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "    double method2(int*);\n"
            "};";
    SetupTempFile( testHeader );

    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );
    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );
    expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );

    // Exercise
    Parser parser;
    bool result = parser.Parse( tempFilePath, *config, true, "", std::vector<std::string>(), error );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( true, result );
    CHECK_EQUAL( 0, error.tellp() );

    // Prepare
    std::ostringstream output1;
    const char* testMock[] = { "### MOCK 1 ###\n", "### MOCK 2 ###\n" };

    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[0] );
    expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[1] );

    // Exercise
    parser.GenerateMock( "", output1 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testMock[0], output1.str().c_str() );
    STRCMP_CONTAINS( testMock[1], output1.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output2;
    const char* testExpect1[] = { "### EXPECT H 1 ###\n", "### EXPECT H 2 ###\n" };

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[0] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[1] );

    // Exercise
    parser.GenerateExpectationHeader( "", output2 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect1[0], output2.str().c_str() );
    STRCMP_CONTAINS( testExpect1[1], output2.str().c_str() );

    // Cleanup
    mock().clear();

    // Prepare
    std::ostringstream output3;
    const char* testExpect2[] = { "### EXPECT I 1 ###\n", "### EXPECT I 2 ###\n" };

    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[0] );
    expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[1] );

    // Exercise
    parser.GenerateExpectationImpl( "", "my_header.h", output3 );

    // Verify
    mock().checkExpectations();
    STRCMP_CONTAINS( testExpect2[0], output3.str().c_str() );
    STRCMP_CONTAINS( testExpect2[1], output3.str().c_str() );
    STRCMP_CONTAINS( "include \"my_header.h\"", output3.str().c_str() );
}

/*
 * Check that a syntax error aborts mock generation.
 */
TEST( Parser, SyntaxError )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   SimpleString testHeader =
           "foo function1(int a);";
   SetupTempFile( testHeader );

   expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilePath, *config, false, "", std::vector<std::string>(), error );

   // Verify
   CHECK_EQUAL( false, result );
   STRCMP_CONTAINS( "PARSE ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_Parser.h:1:1: error: unknown type name 'foo'", error.str().c_str() );

   // Cleanup
}

/*
 * Check that a warning is handled without aborting mock generation.
 */
TEST( Parser, Warning )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   SimpleString testHeader =
           "#warning test\n"
           "void function1(int a);";
   SetupTempFile( testHeader );

   expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );
   expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilePath, *config, false, "", std::vector<std::string>(), error );

   // Verify
   mock().checkExpectations();
   CHECK_EQUAL( true, result );
   STRCMP_CONTAINS( "PARSE WARNING:", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_Parser.h:1:2: warning: test [-W#warnings]", error.str().c_str() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output1;
   const char* testMock = "###MOCK775434578###";
   expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

   // Exercise
   parser.GenerateMock( "", output1 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testMock, output1.str().c_str() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output2;
   const char* testExpectation1 = "###EXPECT38484578###";
   expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpectation1 );

   // Exercise
   parser.GenerateExpectationHeader( "", output2 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testExpectation1, output2.str().c_str() );

   // Cleanup
   mock().clear();

   // Prepare
   std::ostringstream output3;
   const char* testExpectation2 = "###EXPECT993617###";
   expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpectation2 );

   // Exercise
   parser.GenerateExpectationImpl( "", "blabla.h", output3 );

   // Verify
   mock().checkExpectations();
   STRCMP_CONTAINS( testExpectation2, output3.str().c_str() );
   STRCMP_CONTAINS( "include \"blabla.h\"", output3.str().c_str() );
}

/*
 * Check that an error is issued when the input file does not exist.
 */
TEST( Parser, NonExistingInputFile )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   std::filesystem::remove( nonexistingFilePath );

   expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   Parser parser;
   bool result = parser.Parse( nonexistingFilePath, *config, false, "", std::vector<std::string>(), error );

   // Verify
   CHECK_EQUAL( false, result );
   STRCMP_CONTAINS( "INPUT ERROR: Input file '", error.str().c_str() );
   STRCMP_CONTAINS( "CppUMockGen_Parser_NotExisting.h' does not exist", error.str().c_str() );

   // Cleanup
}

/*
 * Check that include paths are processed properly.
 */
TEST( Parser, IncludePaths )
{
   // Prepare
   Config* config = GetMockConfig();
   std::ostringstream error;

   std::string includePath = (std::filesystem::path(PROD_DIR) / "sources").generic_string();

   SimpleString testHeader =
           "#include \"Config.hpp\"\n"
           "void method1(Config &c);\n";
   SetupTempFile( testHeader );

   std::filesystem::current_path( tempDirPath );

   expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

   // Exercise
   Parser parser;
   bool result = parser.Parse( tempFilename, *config, true, "", std::vector<std::string>{includePath}, error );

   // Verify
   CHECK_EQUAL( true, result );
   CHECK_EQUAL( 0, error.tellp() );

   // Cleanup
}

/*
 * Check that regeneration options are printed properly.
 */
TEST( Parser, WithRegenOpts )
{
   // Prepare
   std::ostringstream output;
   const char* testRegenOpts = "####REGEN_OPTS######";


   // Exercise
   Parser parser;
   parser.GenerateMock( testRegenOpts, output );

   // Verify
   STRCMP_CONTAINS( StringFromFormat( "Generation options: %s", testRegenOpts ).asCharString(), output.str().c_str() );

   // Cleanup
}
