/**
 * @file
 * @brief      Unit tests for the "Method" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
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

            Method method;
            if( method.Parse( cursor, config ) )
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
 * Check that a public method with definition inside the class declaration is not mocked.
 */
TEST( Method, PublicNonVirtualWithDefinitionInsideClass )
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
 * Check that a non-virtual protected method with definition inside the class declaration is not mocked.
 */
TEST( Method, ProtectedVirtualWithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class1();\n"
            "    ~class1();\n"
            "protected:\n"
            "    virtual bool method1() const { return true; }\n"
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
 * Check that a non-virtual private method is not mocked.
 */
TEST( Method, NonVirtualPrivateMethod )
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
 * Check that a non-virtual protected method is not mocked.
 */
TEST( Method, NonVirtualProtectedMethod )
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
 * Check that a non-virtual public method is mocked properly.
 */
TEST( Method, NonVirtualPublicMethod )
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
                  "    mock().actualCall(\"class1::method1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual public method is mocked properly.
 */
TEST( Method, VirtualPublicMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1()\n{\n"
                  "    mock().actualCall(\"class1::method1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private method is mocked properly.
 */
TEST( Method, VirtualPrivateMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1()\n{\n"
                  "    mock().actualCall(\"class1::method1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private method is mocked properly.
 */
TEST( Method, VirtualProtectedMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1()\n{\n"
                  "    mock().actualCall(\"class1::method1\").onObject(this);\n"
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
                  "    mock().actualCall(\"class1::method1\").onObject(this);\n"
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
                  "    mock().actualCall(\"ns1::class1::method1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

