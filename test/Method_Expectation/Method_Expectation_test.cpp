/**
 * @file
 * @brief      Unit tests for the "Method" class (expectation generation)
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

TEST_GROUP( Method_Expectation )
{
    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config,
                              std::vector<std::string> &resultsProto, std::vector<std::string> &resultsImpl )
    {
        unsigned int methodCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            methodCount++;

            Method method;
            if( method.Parse( cursor, config ) )
            {
                resultsProto.push_back( method.GenerateExpectation(true) );
                resultsImpl.push_back( method.GenerateExpectation(false) );
            }
        } );

        return methodCount;
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that a public method with definition inside the class declaration does not generate expectation helper function.
 */
TEST( Method_Expectation, PublicNonVirtualWithDefinitionInsideClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1() {}\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual protected method with definition inside the class declaration does not generate expectation helper function.
 */
TEST( Method_Expectation, ProtectedVirtualWithDefinitionInsideClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a method with definition outside the class declaration does not generate expectation helper function.
 */
TEST( Method_Expectation, WithDefinitionOutsideClass )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 2, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual private method does not generate expectation helper function.
 */
TEST( Method_Expectation, NonVirtualPrivateMethod )
{
    // Prepare
    Config* config = GetMockConfig();

   SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    void method1();\n"
            "};";

    // Exercise
   std::vector<std::string> resultsProto;
   std::vector<std::string> resultsImpl;
   unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual protected method does not generate expectation helper function.
 */
TEST( Method_Expectation, NonVirtualProtectedMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a pure virtual method does not generate expectation helper function.
 */
TEST( Method_Expectation, PureVirtualMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual void method1() = 0;\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a public method in a private class does not generate expectation helper function.
 */
TEST( Method_Expectation, PublicMethodInPrivateClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    class class2 {\n"
            "        public:\n"
            "        void method1();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a public method in a protected class does not generate expectation helper function.
 */
TEST( Method_Expectation, PublicMethodInProtectedClass )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    class class2 {\n"
            "        public:\n"
            "        void method1();\n"
            "    };\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a non-virtual public method generates a expectation helper function.
 */
TEST( Method_Expectation, NonVirtualPublicMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual public method generates a expectation helper function.
 */
TEST( Method_Expectation, VirtualPublicMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private method generates a expectation helper function.
 */
TEST( Method_Expectation, VirtualPrivateMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "private:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a virtual private method generates a expectation helper function.
 */
TEST( Method_Expectation, VirtualProtectedMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "protected:\n"
            "    virtual void method1();\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a public const method generates a expectation helper function.
 */
TEST( Method_Expectation, PublicConstMethod )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "class class1 {\n"
            "public:\n"
            "    void method1() const;\n"
            "};";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}

/*
 * Check that a method inside a namespace generates a expectation helper function.
 */
TEST( Method_Expectation, MethodWithinNamespace )
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
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int methodCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, methodCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const ns1::class1*> __object__);\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const ns1::class1*> __object__);\n"
                  "} } }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ { namespace class1$ {\n"
                  "MockExpectedCall& method1(CppUMockGen::Parameter<const ns1::class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectOneCall(\"ns1::class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<ns1::class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "MockExpectedCall& method1(unsigned int __numCalls__, CppUMockGen::Parameter<const ns1::class1*> __object__)\n{\n"
                  "    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, \"ns1::class1::method1\");\n"
                  "    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<ns1::class1*>(__object__.getValue())); }\n"
                  "    return __expectedCall__;\n"
                  "}\n"
                  "} } }\n", resultsImpl[0].c_str() );

    // Cleanup
}
