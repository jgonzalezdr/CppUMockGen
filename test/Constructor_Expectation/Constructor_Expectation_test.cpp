/**
 * @file
 * @brief      Unit tests for the "Constructor" class (expectation generation)
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

TEST_GROUP( Constructor_Expectation )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config,
                              std::vector<std::string> &resultsProto, std::vector<std::string> &resultsImpl )
    {
        unsigned int constructorCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            constructorCount++;

            Constructor constructor;
            if( constructor.Parse( cursor, config ) )
            {
                resultsProto.push_back( constructor.GenerateExpectation(true) );
                resultsImpl.push_back( constructor.GenerateExpectation(false) );
            }
        } );

        return constructorCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a constructor with definition inside the class declaration does not generate expectation helper function.
 */
TEST( Constructor_Expectation, WithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    class1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a constructor with definition outside the class declaration does not generate expectation helper function.
 */
TEST( Constructor_Expectation, WithDefinitionOutsideClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 2, constructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a private constructor does not generate expectation helper function.
 */
TEST( Constructor_Expectation, PrivateConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

   SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class1();\n"
            "};";

    // Exercise
   std::vector<std::string> resultsProto;
   std::vector<std::string> resultsImpl;
   unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a constructor in a private class does not generate expectation helper function.
 */
TEST( Constructor_Expectation, ConstructorInPrivateClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a constructor in a protected class does not generate expectation helper function.
 */
TEST( Constructor_Expectation, ConstructorInProtectedClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a public constructor generates a expectation helper function.
 */
TEST( Constructor_Expectation, PublicConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor();\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor()\n{\n"
                  "    return class1$ctor(1);\n"
                  "}\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::class1\");\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a protected constructor generates a expectation helper function.
 */
TEST( Constructor_Expectation, ProtectedConstructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor();\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor()\n{\n"
                  "    return class1$ctor(1);\n"
                  "}\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::class1\");\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a constructor inside a namespace generates a expectation helper function.
 */
TEST( Constructor_Expectation, ConstructorWithinNamespace )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int constructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, constructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor();\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__);\n"
                  "} } }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& class1$ctor()\n{\n"
                  "    return class1$ctor(1);\n"
                  "}\n"
                  "MockExpectedCall& class1$ctor(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"ns1::class1::class1\");\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} } }\n", resultsImpl[0].c_str() );

    // Cleanup
}
