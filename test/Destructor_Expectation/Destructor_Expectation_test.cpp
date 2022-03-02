/**
 * @file
 * @brief      Unit tests for the "Destructor" class (expectation generation)
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

TEST_GROUP( Destructor_Expectation )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config,
                              std::vector<std::string> &resultsProto, std::vector<std::string> &resultsImpl )
    {
        unsigned int destructorCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            destructorCount++;

            Destructor destructor;
            if( destructor.Parse( cursor, config ) )
            {
                resultsProto.push_back( destructor.GenerateExpectation(true) );
                resultsImpl.push_back( destructor.GenerateExpectation(false) );
            }
        } );

        return destructorCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a destructor with definition inside the class declaration does not generate expectation helper function.
 */
TEST( Destructor_Expectation, NonVirtualWithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    ~class1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a virtual destructor with definition inside the class declaration does not generate expectation helper function.
 */
TEST( Destructor_Expectation, VirtualWithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual ~class1() {};\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual destructor with definition outside the class declaration does not generate expectation helper function.
 */
TEST( Destructor_Expectation, NonVirtualWithDefinitionOutsideClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 2, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a virtual destructor with definition outside the class declaration does not generate expectation helper function.
 */
TEST( Destructor_Expectation, VirtualWithDefinitionOutsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual ~class1();\n"
            "};\n"
            "class1::~class1() {}";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 2, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a destructor in a private class does not generate expectation helper function.
 */
TEST( Destructor_Expectation, DestructorInPrivateClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a destructor in a protected class does not generate expectation helper function.
 */
TEST( Destructor_Expectation, DestructorInProtectedClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual public destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, NonVirtualPublicDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual public destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, VirtualPublicDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a non-virtual private destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, NonVirtualPrivateDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, VirtualPrivateDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a non-virtual protected destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, NonVirtualProtectedDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual protected destructor generates a expectation helper function.
 */
TEST( Destructor_Expectation, VirtualProtectedDestructor )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    virtual ~class1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a destructor inside a namespace generates a expectation helper function.
 */
TEST( Destructor_Expectation, DestructorWithinNamespace )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int destructorCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, destructorCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const ns1::class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const ns1::class1*> __object__ = ::CppUMockGen::IgnoreParameter::YES);\n"
                  "} } }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& class1$dtor(CppUMockGen::Parameter<const ns1::class1*> __object__)\n{\n"
                  "    return class1$dtor(1, __object__);\n"
                  "}\n"
                  "MockExpectedCall& class1$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const ns1::class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"ns1::class1::~class1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<ns1::class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} } }\n", resultsImpl[0].c_str() );

    // Cleanup
}
