/**
 * @file
 * @brief      Unit tests for the "Method" class (mock generation)
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>
#include <functional>

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

TEST_GROUP( Method_Mock )
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
 * Check that a non-virtual method with definition inside the class declaration is not mocked.
 */
TEST( Method_Mock, NonVirtualWithDefinitionInsideClass )
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
 * Check that a virtual method with definition inside the class declaration is not mocked.
 */
TEST( Method_Mock, VirtualWithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
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
 * Check that a non-virtual method with definition outside the class declaration is not mocked.
 */
TEST( Method_Mock, NonVirtualWithDefinitionOutsideClass )
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
 * Check that a virtual method with definition outside the class declaration is not mocked.
 */
TEST( Method_Mock, VirtualWithDefinitionOutsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual void method1();\n"
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
TEST( Method_Mock, NonVirtualPrivateMethod )
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
TEST( Method_Mock, NonVirtualProtectedMethod )
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
TEST( Method_Mock, PureVirtualMethod )
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
 * Check that a method in a private class is not mocked.
 */
TEST( Method_Mock, MethodInPrivateClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class class2 {\n"
            "    public:\n"
            "        void method1();\n"
            "    };\n"
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
 * Check that a method in a protected class is not mocked.
 */
TEST( Method_Mock, MethodInProtectedClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class class2 {\n"
            "    public:\n"
            "        void method1();\n"
            "    };\n"
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
TEST( Method_Mock, NonVirtualPublicMethod )
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
TEST( Method_Mock, VirtualPublicMethod )
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
TEST( Method_Mock, VirtualPrivateMethod )
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
 * Check that a virtual protected method is mocked properly.
 */
TEST( Method_Mock, VirtualProtectedMethod )
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
TEST( Method_Mock, PublicConstMethod )
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
 * Check that a method with exception specifiers is mocked properly.
 */
TEST( Method_Mock, ExceptionSpecifiersMethod )
{
    const std::vector< std::pair< std::string, std::string > > exceptionSpecifiers =
    {
        { "noexcept", "noexcept" },
        { "throw()", "throw()" },
        { "throw(...)", "throw(...)" },
        { "throw(int)", "throw(__put_exception_types_manually_here__)" },
    };

    for( auto exceptionSpecifier : exceptionSpecifiers )
    {
        // Prepare
        Config* config = GetMockConfig();

        SimpleString testHeader = StringFromFormat(
                "class class1 {\n"
                "public:\n"
                "    void method1() %s;\n"
                "};", exceptionSpecifier.first.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int methodCount = ParseHeader( testHeader, *config, results );

        // Verify
        CHECK_EQUAL( 1, methodCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectation = StringFromFormat(
                "void class1::method1() %s\n{\n"
                "    mock().actualCall(\"class1::method1\").onObject(this);\n"
                "}\n", exceptionSpecifier.second.c_str() );
        STRCMP_EQUAL( expectation.asCharString(), results[0].c_str() );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check that a const method with exception specifiers is mocked properly.
 */
TEST( Method_Mock, ExceptionSpecifiersConstMethod )
{
    const std::vector< std::pair< std::string, std::string > > exceptionSpecifiers =
    {
        { "noexcept", "noexcept" },
        { "throw()", "throw()" },
        { "throw(...)", "throw(...)" },
        { "throw(int)", "throw(__put_exception_types_manually_here__)" },
    };

    for( auto exceptionSpecifier : exceptionSpecifiers )
    {
        // Prepare
        Config* config = GetMockConfig();

        SimpleString testHeader = StringFromFormat(
                "class class1 {\n"
                "public:\n"
                "    void method1() const %s;\n"
                "};", exceptionSpecifier.first.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int methodCount = ParseHeader( testHeader, *config, results );

        // Verify
        CHECK_EQUAL( 1, methodCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectation = StringFromFormat(
                "void class1::method1() const %s\n{\n"
                "    mock().actualCall(\"class1::method1\").onObject(this);\n"
                "}\n", exceptionSpecifier.second.c_str() );
        STRCMP_EQUAL( expectation.asCharString(), results[0].c_str() );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check that a method inside a namespace is mocked properly.
 */
TEST( Method_Mock, MethodWithinNamespace )
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

/*
 * Check that a static public method is mocked properly.
 */
TEST( Method_Mock, PublicStaticMethod )
{
    // Prepare
    Config *config = GetMockConfig();

    SimpleString testHeader =
        "class class1 {\n"
        "public:\n"
        "    static void method1();\n"
        "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int methodCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void class1::method1()\n{\n"
                  "    mock().actualCall(\"class1::method1\");\n"
                  "}\n", results[ 0 ].c_str() );

    // Cleanup
}
