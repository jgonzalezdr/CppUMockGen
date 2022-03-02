/**
 * @file
 * @brief      Unit tests for the "Constructor" class (mock generation)
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

#include "Constructor.hpp"

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

TEST_GROUP( Constructor_Mock )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config, std::vector<std::string> &results )
    {
        unsigned int constructorCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            constructorCount++;

            Constructor constructor;
            if( constructor.Parse( cursor, config ) )
            {
                results.push_back( constructor.GenerateMock() );
            }
        } );

        return constructorCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a constructor with definition inside the class declaration is not mocked.
 */
TEST( Constructor_Mock, WithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    class1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a constructor with definition outside the class declaration is not mocked.
 */
TEST( Constructor_Mock, WithDefinitionOutsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    class1();\n"
            "};\n"
            "class1::class1() {}";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 2, constructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a private constructor is not mocked.
 */
TEST( Constructor_Mock, PrivateConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

   SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a constructor in a private class is not mocked.
 */
TEST( Constructor_Mock, ConstructorInPrivateClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class class2 {\n"
            "    public:\n"
            "        class2();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a constructor in a protected class is not mocked.
 */
TEST( Constructor_Mock, ConstructorInProtectedClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class class2 {\n"
            "    public:\n"
            "        class2();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a public constructor is mocked properly.
 */
TEST( Constructor_Mock, PublicConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::class1()\n{\n"
                  "    mock().actualCall(\"class1::class1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a protected constructor is mocked properly.
 */
TEST( Constructor_Mock, ProtectedConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::class1()\n{\n"
                  "    mock().actualCall(\"class1::class1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a constructor with exception specifiers is mocked properly.
 */
TEST( Constructor_Mock, ExceptionSpecifiersConstructor )
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
                "    class1() %s;\n"
                "};", exceptionSpecifier.first.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int constructorCount = ParseHeader( testHeader, *config, results );

        // Verify
        CHECK_EQUAL( 1, constructorCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectation = StringFromFormat(
                "class1::class1() %s\n{\n"
                "    mock().actualCall(\"class1::class1\");\n"
                "}\n", exceptionSpecifier.second.c_str() );
        STRCMP_EQUAL( expectation.asCharString(), results[0].c_str() );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check that a constructor inside a namespace is mocked properly.
 */
TEST( Constructor_Mock, ConstructorWithinNamespace )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "namespace ns1 {\n"
            "class class1 {\n"
            "public:\n"
            "    class1();\n"
            "};\n"
            "}";

    // Exercise
    std::vector<std::string> results;
    unsigned int constructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "ns1::class1::class1()\n{\n"
                  "    mock().actualCall(\"ns1::class1::class1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}
