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
#include "Subtest.hpp"

#include "Parser.hpp"
#include "FileHelper.hpp"
#include "OutputFileParser.hpp"

#include "Config_expect.hpp"
#include "Function_expect.hpp"
#include "Method_expect.hpp"
#include "Constructor_expect.hpp"
#include "Destructor_expect.hpp"
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

static const std::string EMPTY_STRING;

static const std::filesystem::path tempDirPath = std::filesystem::temp_directory_path();
static const std::string defaultTempFilename = "CppUMockGen_Parser.h";
static const std::string nonexistingFilePath = ( tempDirPath / "CppUMockGen_Parser_NotExisting.h" ).generic_string();

TEST_GROUP( Parser )
{
    std::ofstream tempFile;
    std::filesystem::path initialDir;
    std::string tempFilePath;

    TEST_SETUP()
    {
        initialDir = std::filesystem::current_path();
    }

    TEST_TEARDOWN()
    {
        std::filesystem::current_path( initialDir );

        if( !tempFilePath.empty() )
        {
            std::filesystem::remove( tempFilePath );
        }
    }

    void SetupTempFile( const SimpleString& contents )
    {
        SetupTempFile( defaultTempFilename, contents );
    }

    void SetupTempFile( std::string filename, const SimpleString& contents )
    {
        auto tempPath = ( tempDirPath / filename );
        
        std::filesystem::create_directories( tempPath.parent_path() );
        std::filesystem::remove( tempPath );

        tempFile.open( tempPath );
        tempFile << contents.asCharString();
        tempFile.close();
        
        tempFilePath = tempPath.generic_string();
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
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "void function1(int a);";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK6768###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "extern \"C\" {\n#include \"CppUMockGen_Parser.h\"\n}\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1 = "###EXPECT3178###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "extern \"C\" {\n#include \"CppUMockGen_Parser.h\"\n}\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2 = "###EXPECT3682###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a method works as expected.
 */
TEST( Parser, MockedMethod )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    void method1();\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a constructor works as expected.
 */
TEST( Parser, MockedConstructor )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    class1();\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Constructor$::Constructor$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a destructor works as expected.
 */
TEST( Parser, MockedDestructor )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    ~class1();\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Destructor$::Destructor$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that passing a language standard works as expected.
 */
TEST( Parser, MockedMethod_Cpp17 )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config *config = GetMockConfig();
        std::ostringstream error;
        std::string languageStandard = "c++17";

        SimpleString testHeader =
            "#include <filesystem>\n"
            "class class1 {\n"
            "public:\n"
            "    bool method1(std::filesystem::path &p);\n"
            "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &languageStandard );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char *testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char *testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char *testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that passing a C++ language standard implicitly set the language to C++.
 */
TEST( Parser, MockedMethod_Cpp14 )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config *config = GetMockConfig();
        std::ostringstream error;
        std::string languageStandard = "c++14";

        SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    bool method1();\n"
            "};";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &languageStandard );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char *testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char *testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char *testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that passing a GNU++ language standard implicitly set the language to C++.
 */
TEST( Parser, MockedMethod_Gnu98 )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config *config = GetMockConfig();
        std::ostringstream error;
        std::string languageStandard = "gnu++98";

        SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    bool method1();\n"
            "};";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &languageStandard );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char *testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char *testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char *testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a method works as expected.
 */
TEST( Parser, MultipleMockableFunctionsAndMethods )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

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

        expect::Config$::InterpretAsCpp( config, true );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor(4);
        expect::Method$::Method$ctor(2);
        expect::Function$::Parse( 4, IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock[] = { "### MOCK 1 ###\n", "### MOCK 2 ###\n", "### MOCK 3 ###\n", "### MOCK 4 ###\n" };

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[0] );
        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[1] );
        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[2] );
        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[3] );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock[0], output1.str().c_str() );
        STRCMP_CONTAINS( testMock[1], output1.str().c_str() );
        STRCMP_CONTAINS( testMock[2], output1.str().c_str() );
        STRCMP_CONTAINS( testMock[3], output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1[] = { "### EXPECT H 1 ###\n", "### EXPECT H 2 ###\n", "### EXPECT H 3 ###\n", "### EXPECT H 4 ###\n" };

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[0] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[1] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[2] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[3] );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1[0], output2.str().c_str() );
        STRCMP_CONTAINS( testExpect1[1], output2.str().c_str() );
        STRCMP_CONTAINS( testExpect1[2], output2.str().c_str() );
        STRCMP_CONTAINS( testExpect1[3], output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2[] = { "### EXPECT I 1 ###\n", "### EXPECT I 2 ###\n", "### EXPECT I 3 ###\n", "### EXPECT I 4 ###\n" };

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[0] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[1] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[2] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[3] );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2[0], output3.str().c_str() );
        STRCMP_CONTAINS( testExpect2[1], output3.str().c_str() );
        STRCMP_CONTAINS( testExpect2[2], output3.str().c_str() );
        STRCMP_CONTAINS( testExpect2[3], output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor(4);

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a non-mockable function works as expected.
 */
TEST( Parser, FunctionNonMockable )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "void function1(int a) {};";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Function$dtor();

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
        STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a non-mockable method works as expected.
 */
TEST( Parser, MethodNonMockable )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    void method1() {};\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Function$dtor();

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
        STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a non-mockable constructor works as expected.
 */
TEST( Parser, ConstructorNonMockable )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    class1() {};\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Constructor$::Constructor$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Function$dtor();

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
        STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mocking a non-mockable destructor works as expected.
 */
TEST( Parser, DestructorNonMockable )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    ~class1() {};\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Destructor$::Destructor$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Function$dtor();

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "INPUT ERROR:", error.str().c_str() );
        STRCMP_CONTAINS( "The input file does not contain any mockable function", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that mixed mockable and non-mockable methods works as expected.
 */
TEST( Parser, MixedMockableNonMockableFunctionsAndMethods )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "void function1(int a) {};\n"
                "int function2();\n"
                "class class1 {\n"
                "public:\n"
                "    void method1() {};\n"
                "    double method2(int*);\n"
                "};";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, true );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor( 4 );
        expect::Method$::Method$ctor( 2 );
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, false );
        expect::Function$::Function$dtor(2);

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock[] = { "### MOCK 1 ###\n", "### MOCK 2 ###\n" };

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[0] );
        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock[1] );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock[0], output1.str().c_str() );
        STRCMP_CONTAINS( testMock[1], output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpect1[] = { "### EXPECT H 1 ###\n", "### EXPECT H 2 ###\n" };

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[0] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1[1] );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1[0], output2.str().c_str() );
        STRCMP_CONTAINS( testExpect1[1], output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpect2[] = { "### EXPECT I 1 ###\n", "### EXPECT I 2 ###\n" };

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[0] );
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2[1] );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2[0], output3.str().c_str() );
        STRCMP_CONTAINS( testExpect2[1], output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor(2);

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that a syntax error aborts mock generation.
 */
TEST( Parser, SyntaxError )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "foo function1(int a);";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "PARSE ERROR:", error.str().c_str() );
        STRCMP_CONTAINS( "CppUMockGen_Parser.h:1:1: error: unknown type name 'foo'", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that a warning is handled without aborting mock generation.
 */
TEST( Parser, Warning )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        SimpleString testHeader =
                "#warning test\n"
                "void function1(int a);";
        SetupTempFile( testHeader );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        STRCMP_CONTAINS( "PARSE WARNING:", error.str().c_str() );
        STRCMP_CONTAINS( "CppUMockGen_Parser.h:1:2: warning: test [-W#warnings]", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK775434578###";
        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", "", output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )

        // Prepare
        std::ostringstream output2;
        const char* testExpectation1 = "###EXPECT38484578###";
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpectation1 );

        // Exercise
        parser->GenerateExpectationHeader( "", "", output2 );

        // Verify
        STRCMP_CONTAINS( testExpectation1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_Parser.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )

        // Prepare
        std::ostringstream output3;
        const char* testExpectation2 = "###EXPECT993617###";
        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpectation2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "blabla.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpectation2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"blabla.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that an error is issued when the input file does not exist.
 */
TEST( Parser, NonExistingInputFile )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        std::filesystem::remove( nonexistingFilePath );

        expect::Config$::InterpretAsCpp( config, false );
        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );

        expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

        // Exercise
        bool result = parser->Parse( nonexistingFilePath, *config, false, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( false, result );
        STRCMP_CONTAINS( "INPUT ERROR: Input file '", error.str().c_str() );
        STRCMP_CONTAINS( "CppUMockGen_Parser_NotExisting.h' does not exist", error.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that include paths are processed properly.
 */
TEST( Parser, IncludePaths )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;

        std::string includePath = (std::filesystem::path(PROD_DIR) / "sources").generic_string();

        SimpleString testHeader =
                "#include \"Config.hpp\"\n"
                "void function1(Config &c);\n";
        SetupTempFile( testHeader );

        std::filesystem::current_path( tempDirPath );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( defaultTempFilename, *config, true, std::vector<std::string>{includePath}, error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that regeneration options are stored properly.
 */
TEST( Parser, WithRegenOpts )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        std::ostringstream output;
        const char* testRegenOpts = "####REGEN_OPTS######";

        // Exercise
        parser->GenerateMock( testRegenOpts, "", "", output );

        // Verify
        STRCMP_CONTAINS( StringFromFormat( "Generation options: %s", testRegenOpts ).asCharString(), output.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that base directory setting works as expected.
 */
TEST( Parser, BaseDirectory )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        Config* config = GetMockConfig();
        std::ostringstream error;
        std::string inputFileName = "CppUMockGen_foo/CppUMockGen_bar.h";

        SimpleString testHeader =
                "class class1 {\n"
                "public:\n"
                "    void method1();\n"
                "};";
        SetupTempFile( inputFileName, testHeader );

        expect::Config$::GetLanguageStandard( config, &EMPTY_STRING );
        expect::Function$::Function$ctor();
        expect::Method$::Method$ctor();
        expect::Function$::Parse( IgnoreParameter::YES, IgnoreParameter::YES, config, true );

        // Exercise
        bool result = parser->Parse( tempFilePath, *config, true, std::vector<std::string>(), error );

        // Verify
        CHECK_EQUAL( true, result );
        CHECK_EQUAL( 0, error.tellp() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateMock invocation" )

        // Prepare
        std::ostringstream output1;
        const char* testMock = "###MOCK###";

        expect::Function$::GenerateMock( IgnoreParameter::YES, testMock );

        // Exercise
        parser->GenerateMock( "", "", tempDirPath, output1 );

        // Verify
        STRCMP_CONTAINS( testMock, output1.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_foo/CppUMockGen_bar.h\"\n", output1.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationHeader invocation" )
    
        // Prepare
        std::ostringstream output2;
        const char* testExpect1 = "###EXPECT87828763###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, true, testExpect1 );

        // Exercise
        parser->GenerateExpectationHeader( "", tempDirPath, output2 );

        // Verify
        STRCMP_CONTAINS( testExpect1, output2.str().c_str() );
        STRCMP_CONTAINS( "#include \"CppUMockGen_foo/CppUMockGen_bar.h\"\n", output2.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::GenerateExpectationImpl invocation" )
    
        // Prepare
        std::ostringstream output3;
        const char* testExpect2 = "###EXPECT87362###";

        expect::Function$::GenerateExpectation( IgnoreParameter::YES, false, testExpect2 );

        // Exercise
        parser->GenerateExpectationImpl( "", "my_header.h", output3 );

        // Verify
        STRCMP_CONTAINS( testExpect2, output3.str().c_str() );
        STRCMP_CONTAINS( "#include \"my_header.h\"\n", output3.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Prepare
        expect::Function$::Function$dtor();

        // Exercise
        delete( parser );

    SUBTEST_END
}

/*
 * Check that user code is stored properly.
 */
TEST( Parser, UserCode )
{
    SUBTEST_BEGIN( "Parser instance creation" )

        // Exercise
        Parser *parser = new Parser();

    SUBTEST_END

    SUBTEST_BEGIN( "Parser::Parse invocation" )

        // Prepare
        std::ostringstream output;
        const char* userCode =
            "User code...\n"
            "spanning more than...\n"
            "one line.\n";

        // Exercise
        parser->GenerateMock( "", userCode, "", output );

        // Verify
        STRCMP_CONTAINS( StringFromFormat( "\n"
                "// " USER_CODE_BEGIN "\n"
                "%s"
                "// " USER_CODE_END "\n",
                userCode ).asCharString(), 
            output.str().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "Parser instance deletion" )

        // Exercise
        delete( parser );

    SUBTEST_END
}
