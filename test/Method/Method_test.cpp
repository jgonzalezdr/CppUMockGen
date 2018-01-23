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

#include <string>
#include <vector>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "Method.hpp"

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

TEST_GROUP( Method )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config, std::vector<std::string> &results )
    {
        unsigned int methodCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            methodCount++;

            Method method( cursor, config );

            if( method.IsMockable() )
            {
                results.push_back( method.GenerateMock() );
            }
        } );

        return methodCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a method with definition inside the class declaration is not mocked.
 */
TEST( Method, WithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a method with definition outside the class declaration is not mocked.
 */
TEST( Method, WithDefinitionOutsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};\n"
            "void class1::method1() {}";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 2, methodCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a private method is not mocked.
 */
TEST( Method, PrivateMethod )
{
    // Prepare
    Config* config = GetMockConfig();

   SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a protected method is not mocked.
 */
TEST( Method, ProtectedMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a pure virtual method is not mocked.
 */
TEST( Method, PureVirtualMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual void method1() = 0;\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a public method is mocked properly.
 */
TEST( Method, PublicMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1()\n{\n"
                  "    mock().actualCall(\"class1::method1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a public const method is mocked properly.
 */
TEST( Method, PublicConstMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1() const;\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1() const\n{\n"
                  "    mock().actualCall(\"class1::method1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a method inside a namespace is mocked properly.
 */
TEST( Method, MethodWithinNamespace )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "namespace ns1 {\n"
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};\n"
            "}";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void ns1::class1::method1()\n{\n"
                  "    mock().actualCall(\"ns1::class1::method1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

