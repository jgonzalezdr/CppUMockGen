/**
 * @file
 * @brief      Unit tests for the "Destructor" class (mock generation)
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

#include "Destructor.hpp"

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

TEST_GROUP( Destructor_Mock )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config, std::vector<std::string> &results )
    {
        unsigned int destructorCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            destructorCount++;

            Destructor destructor;
            if( destructor.Parse( cursor, config ) )
            {
                results.push_back( destructor.GenerateMock() );
            }
        } );

        return destructorCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a destructor with definition inside the class declaration is not mocked.
 */
TEST( Destructor_Mock, WithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    ~class1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a destructor with definition outside the class declaration is not mocked.
 */
TEST( Destructor_Mock, WithDefinitionOutsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    ~class1();\n"
            "};\n"
            "class1::~class1() {}";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 2, destructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a destructor in a private class is not mocked.
 */
TEST( Destructor_Mock, DestructorInPrivateClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class class2 {\n"
            "    public:\n"
            "        ~class2();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a destructor in a protected class is not mocked.
 */
TEST( Destructor_Mock, DestructorInProtectedClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class class2 {\n"
            "    public:\n"
            "        ~class2();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a non-virtual public destructor is mocked properly.
 */
TEST( Destructor_Mock, NonVirtualPublicDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual public destructor is mocked properly.
 */
TEST( Destructor_Mock, VirtualPublicDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a non-virtual private destructor is mocked properly.
 */
TEST( Destructor_Mock, NonVirtualPrivateDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private destructor is mocked properly.
 */
TEST( Destructor_Mock, VirtualPrivateDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a non-virtual protected destructor is mocked properly.
 */
TEST( Destructor_Mock, NonVirtualProtectedDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual protected destructor is mocked properly.
 */
TEST( Destructor_Mock, VirtualProtectedDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "class1::~class1()\n{\n"
                  "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

/*
 * Check that a public destructor with exception specifiers is mocked properly.
 */
TEST( Destructor_Mock, PublicExceptionSpecifiersDestructor )
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
                "    ~class1() %s;\n"
                "};", exceptionSpecifier.first.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int destructorCount = ParseHeader( testHeader, *config, results );

        // Verify
        CHECK_EQUAL( 1, destructorCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectation = StringFromFormat(
                "class1::~class1() %s\n{\n"
                "    mock().actualCall(\"class1::~class1\").onObject(this);\n"
                "}\n", exceptionSpecifier.second.c_str() );
        STRCMP_EQUAL( expectation.asCharString(), results[0].c_str() );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check that a destructor inside a namespace is mocked properly.
 */
TEST( Destructor_Mock, DestructorWithinNamespace )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "namespace ns1 {\n"
            "class class1 {\n"
            "public:\n"
            "    ~class1();\n"
            "};\n"
            "}";

    // Exercise
    std::vector<std::string> results;
    unsigned int destructorCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "ns1::class1::~class1()\n{\n"
                  "    mock().actualCall(\"ns1::class1::~class1\").onObject(this);\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}
