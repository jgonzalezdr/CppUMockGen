/**
 * @file
 * @brief      Unit tests for the "Function" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <vector>
#include <string>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "Function.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

#ifdef INTERPRET_C
#define TEST_GROUP_NAME Function_Expectation_C
#else
#define TEST_GROUP_NAME Function_Expectation_CPP
#endif

#define TEST_GROUP_EX(x) TEST_GROUP(x)
#define TEST_EX(x,y) TEST(x,y)

#ifdef INTERPRET_C
#define ENUM_TAG "enum "
#define STRUCT_TAG "struct "
#else
#define ENUM_TAG
#define STRUCT_TAG
#endif

Config* GetMockConfig( uintptr_t n = 0 )
{
    return (Config*) (void*) (n + 78876433);
}

Config::OverrideSpec* GetMockConfig_OverrideSpec( uintptr_t n = 0 )
{
    return (Config::OverrideSpec*) (void*) (n + 23898746);
}

class StdStringCopier : public MockNamedValueCopier
{
public:
    virtual void copy(void* out, const void* in)
    {
        *(std::string*)out = *(const std::string*)in;
    }
};

StdStringCopier stdStringCopier;

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP_EX( TEST_GROUP_NAME )
{
    struct PrimitiveTypeData
    {
        std::string originalType;
        std::string mockedType;
        bool casted;
        std::string cpputestFunctionType;
    };

    const std::vector< struct PrimitiveTypeData > primitiveTypes {
        { "int", "int", false, "Int" },
        { "unsigned int", "unsigned int", false, "UnsignedInt" },
        { "signed int", "int", false, "Int" },
        { "short", "short", true, "Int" },
        { "unsigned short", "unsigned short", true, "UnsignedInt" },
        { "signed short", "short", true, "Int" },
        { "char", "char", true, "Int" },
        { "unsigned char", "unsigned char", true, "UnsignedInt" },
        { "signed char", "signed char", true, "Int" },
        { "long", "long", false, "LongInt" },
        { "unsigned long", "unsigned long", false, "UnsignedLongInt" },
        { "signed long", "long", false, "LongInt" },
        { "float", "float", true, "Double" },
        { "double", "double", false, "Double" },
#ifndef INTERPRET_C
        { "bool", "bool", false, "Bool" },
        { "wchar_t", "wchar_t", true, "Int" },
        { "char16_t", "char16_t", true, "UnsignedInt" },
        { "char32_t", "char32_t", true, "UnsignedLongInt" },
#endif
    };

    struct PrimitivePointedTypeData
    {
        std::string originalType;
        std::string mockedType;
    };

    const std::vector< struct PrimitivePointedTypeData > primitivePointedTypesWithoutString {
        { "int", "int" },
        { "unsigned int", "unsigned int" },
        { "signed int", "int" },
        { "short", "short" },
        { "unsigned short", "unsigned short" },
        { "signed short", "short" },
        { "long", "long" },
        { "unsigned long", "unsigned long" },
        { "signed long", "long" },
        { "unsigned char", "unsigned char" },
        { "signed char", "signed char" },
        { "float", "float" },
        { "double", "double" },
#ifndef INTERPRET_C
        { "bool", "bool" },
        { "wchar_t", "wchar_t" },
        { "char16_t", "char16_t" },
        { "char32_t", "char32_t" },
#endif
    };

    std::vector< struct PrimitivePointedTypeData > primitivePointedTypesWithString = primitivePointedTypesWithoutString;

    std::vector< struct PrimitivePointedTypeData > primitivePointedTypesWithStringAndVoid = primitivePointedTypesWithoutString;

    std::vector< struct PrimitivePointedTypeData > primitivePointedTypesWithoutStringWithVoid = primitivePointedTypesWithoutString;

    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config,
                              std::vector<std::string> &resultsProto, std::vector<std::string> &resultsImpl )
    {
        unsigned int functionCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function;
            if( function.Parse( cursor, config ) )
            {
                resultsProto.push_back( function.GenerateExpectation(true) );
                resultsImpl.push_back( function.GenerateExpectation(false) );
            }
        } );

        return functionCount;
    }

    TEST_SETUP()
    {
        primitivePointedTypesWithString.insert( primitivePointedTypesWithString.end(), { { "char", "char" } } );
        primitivePointedTypesWithStringAndVoid.insert( primitivePointedTypesWithStringAndVoid.end(), { { "char", "char" }, { "void", "void" } } );
        primitivePointedTypesWithoutStringWithVoid.insert( primitivePointedTypesWithoutStringWithVoid.end(), { { "void", "void" } } );
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

//*************************************************************************************************
//
//                                           GENERIC
//
//*************************************************************************************************

/*
 * Check that a function with definition does not generate expectation.
 */
TEST_EX( TEST_GROUP_NAME, WithDefinition )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function1() {}";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 0, resultsProto.size() );

    // Cleanup
}

/*
 * Check that a function declared twice only generates a single expectation.
 */
TEST_EX( TEST_GROUP_NAME, DoubleDeclaration )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "void function1();\n"
            "void function1();";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 2, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1();\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1()\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check that a function inside a namespace generates a expectation properly.
 */
TEST_EX( TEST_GROUP_NAME, FunctionWithinNamespace )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "namespace ns1 {\n"
            "void function1();\n"
            "}";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ {\n"
                  "MockExpectedCall& function1();\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__);\n"
                  "} }\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect { namespace ns1$ {\n"
                  "MockExpectedCall& function1()\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"ns1::function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"ns1::function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "} }\n", resultsImpl[0].c_str() );

    // Cleanup
}
#endif

//*************************************************************************************************
//
//                                DIFFERENT RETURNS / NO PARAMETERS
//
//*************************************************************************************************

//*************************************************************************************************
//                                          Basic Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters nor return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function1();";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1();\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1()\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with return and an explicit void parameter.
 */
TEST_EX( TEST_GROUP_NAME, ExplicitVoidParameter )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function(void);";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function();\n"
                  "MockExpectedCall& function(unsigned int __numCalls__);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function()\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function(unsigned int __numCalls__)\n{\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function\");\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a primitive type value.
 */
TEST_EX( TEST_GROUP_NAME, PrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@%s", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "%s function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> resultsProto;
        std::vector<std::string> resultsImpl;
        unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, resultsProto.size() );
        STRCMP_EQUAL( "namespace expect {\n"
                      "MockExpectedCall& function1();\n"
                      "MockExpectedCall& function1(unsigned int __numCalls__);\n"
                      "}\n", resultsProto[0].c_str() );
        STRCMP_EQUAL( "namespace expect {\n"
                      "MockExpectedCall& function1()\n{\n"
                      "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                      "    return expectedCall;\n"
                      "}\n"
                      "MockExpectedCall& function1(unsigned int __numCalls__)\n{\n"
                      "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                      "    return expectedCall;\n"
                      "}\n"
                      "}\n", resultsImpl[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

        // Cleanup
        mock().clear();
    }
}

#if 0

/*
 * Check mock generation of a function without parameters and returning an enum value.
 */
TEST_EX( TEST_GROUP_NAME, EnumReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@" ENUM_TAG "Enum1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum Enum1 { A, B, C };\n"
            ENUM_TAG "Enum1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( ENUM_TAG "Enum1 function1()\n{\n"
                  "    return static_cast<" ENUM_TAG "Enum1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a scoped enum value.
 */
TEST_EX( TEST_GROUP_NAME, ScopedEnumReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Enum1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum class Enum1 { A, B, C };\n"
            "Enum1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Enum1 function1()\n{\n"
                  "    return static_cast<Enum1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a class.
 */
TEST_EX( TEST_GROUP_NAME, ClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1 function1()\n{\n"
                  "    return * static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a template class.
 */
TEST_EX( TEST_GROUP_NAME, TemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1<int>").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int> function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1<int> function1()\n{\n"
                  "    return * static_cast<const Class1<int> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a struct.
 */
TEST_EX( TEST_GROUP_NAME, StructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@" STRUCT_TAG "Struct1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            STRUCT_TAG "Struct1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( STRUCT_TAG "Struct1 function1()\n{\n"
                  "    return * static_cast<const " STRUCT_TAG "Struct1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                       Basic Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a typedef for a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Type1 function1()\n{\n"
                "    return static_cast<Type1>( mock().actualCall(\"function1\").return%sValue() );\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for an enum.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForEnumReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef enum { X, Y, Z } Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a typedef for a scoped enum.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForScopedEnumReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z };\n"
            "typedef Enum1 Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a class.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return * static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a template class.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "typedef Class1<long> Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return * static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a typedef for a struct.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "typedef struct Struct1 Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return * static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                       Basic Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const void value.
 */
TEST_EX( TEST_GROUP_NAME, PointerToVoidReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@void *").andReturnValue((const void*)0);

    SimpleString testHeader = "void* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnPointerValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const void value.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstVoidReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const void *").andReturnValue((const void*)0);

    SimpleString testHeader = "const void* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const void * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnConstPointerValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, PointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@%s *", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "%s* function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "%s * function1()\n{\n"
                "    return static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@const %s *", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "const %s* function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "const %s * function1()\n{\n"
                "    return static_cast<const %s *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a string.
 */
TEST_EX( TEST_GROUP_NAME, StringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const char *").andReturnValue((const void*)0);

    SimpleString testHeader = "const char* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const char * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnStringValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1 * function1()\n{\n"
                  "    return static_cast<Class1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "const Class1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Class1 * function1()\n{\n"
                  "    return static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const template class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1<int> *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int>* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1<int> * function1()\n{\n"
                  "    return static_cast<Class1<int> *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const template class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1<char> *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "const Class1<char>* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Class1<char> * function1()\n{\n"
                  "    return static_cast<const Class1<char> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const struct.
 */
TEST_EX( TEST_GROUP_NAME, PointerToStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@struct Struct1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "struct Struct1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "struct Struct1 * function1()\n{\n"
                  "    return static_cast<struct Struct1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const struct.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const struct Struct1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "const struct Struct1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const struct Struct1 * function1()\n{\n"
                  "    return static_cast<const struct Struct1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                   Left-Value Reference Return
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@%s &", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "%s& function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "%s & function1()\n{\n"
                "    return * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@const %s &", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "const %s& function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "const %s & function1()\n{\n"
                "    return * static_cast<const %s *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "Class1 & function1()\n{\n"
            "    return * static_cast<Class1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const class.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToConstClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "const Class1& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const Class1 & function1()\n{\n"
            "    return * static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const template class.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1<int> &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int>& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "Class1<int> & function1()\n{\n"
            "    return * static_cast<Class1<int> *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const template class.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToConstTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1<int> &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "const Class1<int>& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const Class1<int> & function1()\n{\n"
            "    return * static_cast<const Class1<int> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const struct.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@struct Struct1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "struct Struct1& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "struct Struct1 & function1()\n{\n"
            "    return * static_cast<struct Struct1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const struct.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToConstStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const struct Struct1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "const struct Struct1& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const struct Struct1 & function1()\n{\n"
            "    return * static_cast<const struct Struct1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                   Right-Value Reference Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@%s &&", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "%s&& function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "%s && function1()\n{\n"
                "    return std::move( * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "@const %s &&", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "const %s&& function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "const %s && function1()\n{\n"
                "    return std::move( * static_cast<const %s *>( mock().actualCall(\"function1\").returnConstPointerValue() ) );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "Class1 && function1()\n{\n"
            "    return std::move( * static_cast<Class1 *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const class.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToConstClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "const Class1&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const Class1 && function1()\n{\n"
            "    return std::move( * static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const template class.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Class1<int> &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int>&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "Class1<int> && function1()\n{\n"
            "    return std::move( * static_cast<Class1<int> *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const template class.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToConstTemplateClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Class1<int> &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "const Class1<int>&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const Class1<int> && function1()\n{\n"
            "    return std::move( * static_cast<const Class1<int> *>( mock().actualCall(\"function1\").returnConstPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const struct.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@struct Struct1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "struct Struct1&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "struct Struct1 && function1()\n{\n"
            "    return std::move( * static_cast<struct Struct1 *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const struct.
 */
TEST_EX( TEST_GROUP_NAME, RVReferenceToConstStructReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const struct Struct1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "const struct Struct1&& function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "const struct Struct1 && function1()\n{\n"
            "    return std::move( * static_cast<const struct Struct1 *>( mock().actualCall(\"function1\").returnConstPointerValue() ) );\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                   Pointer to Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, PointerToTypedefForPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "Type1* function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 * function1()\n{\n"
                      "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, PointerToTypedefForConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s Type1;\n"
                "Type1* function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 * function1()\n{\n"
                      "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const typedef for a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstTypedefForPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "const Type1* function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type1 * function1()\n{\n"
                      "    return static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a typedef for a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, ConstPointerToTypedefForPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1 *const").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "Type1* const function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 *const function1()\n{\n"
                      "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToTypedefForClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "Type1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 * function1()\n{\n"
                  "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const typedef for a class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToTypedefForConstClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1 Type1;"
            "Type1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 * function1()\n{\n"
                  "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a typedef for a class.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstTypedefForClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "const Type1* function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type1 * function1()\n{\n"
                  "    return static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                   Typedef for Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForPointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 function1()\n{\n"
                      "    return static_cast<Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForPointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 function1()\n{\n"
                      "    return static_cast<Type1>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a const pointer to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForConstPointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* const Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 function1()\n{\n"
                      "    return static_cast<Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a pointer to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForPointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "const Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type1 function1()\n{\n"
                      "    return static_cast<const Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a string.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForStringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a string.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForStringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "const Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type1 function1()\n{\n"
                  "    return static_cast<const Type1>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForPointerToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* Type1;"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a const class.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForPointerToConstClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1* Type1;"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a const pointer to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForConstPointerToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* const Type1;"
            "Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a pointer to a non-const class.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForPointerToClassReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* Type1;"
            "const Type1 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type1 function1()\n{\n"
                  "    return static_cast<const Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                   Reference to Typedef Return
//*************************************************************************************************

//*************************************************************************************************
//                                   Typedef for Reference Return
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a typedef for a left-value reference to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForLVReferenceToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s& Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Type1 function1()\n{\n"
                "    return static_cast<Type1>( * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a right-value reference to a non-const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForRVReferenceToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s&& Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Type1 function1()\n{\n"
                "    return static_cast<Type1>( std::move( * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() ) ) );\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}
#endif

//*************************************************************************************************
//                                     Pointer to Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a non-const pointer to a non-const pointer.
 */
TEST_EX( TEST_GROUP_NAME, PointerToPointerReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@int **").andReturnValue((const void*)0);

    SimpleString testHeader = "int* *function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "int ** function1()\n{\n"
                  "    return static_cast<int * *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a non-const pointer to a const pointer.
 */
TEST_EX( TEST_GROUP_NAME, PointerToConstPointerReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const unsigned char **").andReturnValue((const void*)0);

    SimpleString testHeader = "const unsigned char* *function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const unsigned char ** function1()\n{\n"
                  "    return static_cast<const unsigned char * *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a non-const pointer.
 */
TEST_EX( TEST_GROUP_NAME, ConstPointerToPointerReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@short *const *").andReturnValue((const void*)0);

    SimpleString testHeader = "short* const *function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "short *const * function1()\n{\n"
                  "    return static_cast<short *const *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                             Left-Value Reference to Pointer Return
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const pointer.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToPointerReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@double *&").andReturnValue((const void*)0);

    SimpleString testHeader = "double* &function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "double *& function1()\n{\n"
                  "    return * static_cast<double * *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const pointer.
 */
TEST_EX( TEST_GROUP_NAME, LVReferenceToConstPointerReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@bool *const &").andReturnValue((const void*)0);

    SimpleString testHeader = "bool* const &function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "bool *const & function1()\n{\n"
                  "    return * static_cast<bool *const *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                Typedef for Basic Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a double typedef for a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForTypedefForPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "typedef Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Type2 function1()\n{\n"
                "    return static_cast<Type2>( mock().actualCall(\"function1\").return%sValue() );\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                             Typedef for Typedef for Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForTypedefForPointerToPrimitiveTypePointerReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForConstTypedefForPointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a primitive type.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForTypedefForPointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "const Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type2 function1()\n{\n"
                      "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForTypedefForPointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForConstTypedefForPointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a const primitive type.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForTypedefForPointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "const Type2 function1();",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type2 function1()\n{\n"
                      "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const typedef for a string.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForTypedefForStringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "Type2 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type2 function1()\n{\n"
                  "    return static_cast<Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a string.
 */
TEST_EX( TEST_GROUP_NAME, TypedefForConstTypedefForStringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef const Type1 Type2;\n"
            "Type2 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type2 function1()\n{\n"
                  "    return static_cast<Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a string.
 */
TEST_EX( TEST_GROUP_NAME, ConstTypedefForTypedefForStringReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@const Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "const Type2 function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type2 function1()\n{\n"
                  "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#endif

//*************************************************************************************************
//
//                                 VOID RETURN / SINGLE PARAMETER
//
//*************************************************************************************************

//*************************************************************************************************
//                                      Simple Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#%s", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat( "void function1(%s p);", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> resultsProto;
        std::vector<std::string> resultsImpl;
        unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, resultsProto.size() );
        SimpleString expectedResultProto = StringFromFormat(
                "namespace expect {\n"
                "MockExpectedCall& function1(CppUMockGen::Parameter<%s> p);\n"
                "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<%s> p);\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        SimpleString expectedResultImpl = StringFromFormat(
                "namespace expect {\n"
                "MockExpectedCall& function1(CppUMockGen::Parameter<%s> p)\n{\n"
                "    bool ignoreOtherParams = false;\n"
                "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.with%sParameter(\"p\", p.getValue()); }\n"
                "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                "    return expectedCall;\n"
                "}\n"
                "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<%s> p)\n{\n"
                "    bool ignoreOtherParams = false;\n"
                "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.with%sParameter(\"p\", p.getValue()); }\n"
                "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                "    return expectedCall;\n"
                "}\n"
                "}\n",
                typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str(),
                typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResultProto.asCharString(), resultsProto[0].c_str() );
        STRCMP_EQUAL( expectedResultImpl.asCharString(), resultsImpl[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with an enum parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnEnumParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#" ENUM_TAG "Enum1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum Enum1 { X, Y, Z };\n"
            "void function1(" ENUM_TAG "Enum1 p);";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<" ENUM_TAG "Enum1> p);\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<" ENUM_TAG "Enum1> p);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<" ENUM_TAG "Enum1> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withIntParameter(\"p\", static_cast<int>(p.getValue())); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<" ENUM_TAG "Enum1> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withIntParameter(\"p\", static_cast<int>(p.getValue())); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a scoped enum parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnScopedEnumParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Enum1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z };\n"
            "void function1(Enum1 p);";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Enum1> p);\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Enum1> p);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Enum1> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withIntParameter(\"p\", static_cast<int>(p.getValue())); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Enum1> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withIntParameter(\"p\", static_cast<int>(p.getValue())); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1 p);";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Class1&> p);\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Class1&> p);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Class1&> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withParameterOfType(\"Class1\", \"p\", &p.getValue()); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Class1&> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withParameterOfType(\"Class1\", \"p\", &p.getValue()); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1<short>").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short> p);";

    // Exercise
    std::vector<std::string> resultsProto;
    std::vector<std::string> resultsImpl;
    unsigned int functionCount = ParseHeader( testHeader, *config, resultsProto, resultsImpl );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, resultsProto.size() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Class1<short>&> p);\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Class1<short>&> p);\n"
                  "}\n", resultsProto[0].c_str() );
    STRCMP_EQUAL( "namespace expect {\n"
                  "MockExpectedCall& function1(CppUMockGen::Parameter<Class1<short>&> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectOneCall(\"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withParameterOfType(\"Class1<short>\", \"p\", &p.getValue()); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "MockExpectedCall& function1(unsigned int __numCalls__, CppUMockGen::Parameter<Class1<short>&> p)\n{\n"
                  "    bool ignoreOtherParams = false;\n"
                  "    MockExpectedCall& expectedCall = mock().expectNCalls(__numCalls__, \"function1\");\n"
                  "    if(p.isIgnored()) { ignoreOtherParams = true; } else { expectedCall.withParameterOfType(\"Class1<short>\", \"p\", &p.getValue()); }\n"
                  "    if(ignoreOtherParams) { expectedCall.ignoreOtherParameters(); }\n"
                  "    return expectedCall;\n"
                  "}\n"
                  "}\n", resultsImpl[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsProto[0] ) );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), resultsImpl[0] ) );

    // Cleanup
}
#endif

#if 0

/*
 * Check mock generation of a function with a struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#" STRUCT_TAG "Struct1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(" STRUCT_TAG "Struct1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(" STRUCT_TAG "Struct1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Struct1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                  Simple Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(Type1 p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for an enum parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForEnumParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef enum { X, Y, Z, K } Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a typedef for a scoped enum parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForScopedEnumParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z, W };\n"
            "typedef Enum1 Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForClassParameter_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(true);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "typedef Class1<long> Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTemplateClassParameter_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(true);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "typedef Class1<long> Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<long>\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function with a typedef for a struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "typedef struct Struct1 Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForStructParameter_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(true);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "typedef struct Struct1 Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Struct1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                   Simple Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to non-const void parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToVoidParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#void *").andReturnValue((const void*)0);

    SimpleString testHeader = "void function1(void* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(void * p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#%s *", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(%s* p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(%s * p)\n{\n"
                "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#const %s *", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(const %s* p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(const %s * p)\n{\n"
                "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const char *").andReturnValue((const void*)0);

    SimpleString testHeader = "void function1(const char* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const char * p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a pointer to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(const Class1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Class1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a non-const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1<short> *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short>* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1<short> * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1<short>\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1<short> *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(const Class1<short>* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Class1<short> * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function with a pointer to a non-const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#struct Struct1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(struct Struct1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(struct Struct1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Struct1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const struct Struct1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(const struct Struct1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const struct Struct1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Struct1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                              Simple Left-Value Reference Parameters
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a left-value reference to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#%s &", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(%s& p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                  "void function1(%s & p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameter(\"p\", &p);\n"
                  "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a left-value reference to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#const %s &", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(const %s& p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                  "void function1(const %s & p)\n{\n"
                  "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", &p);\n"
                  "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a left-value reference to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(Class1 & p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToConstClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(const Class1& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const Class1 & p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a non-const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1<short> &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short>& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(Class1<short> & p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1<short>\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToConstTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1<short> &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(const Class1<short>& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const Class1<short> & p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a non-const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#struct Struct1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(struct Struct1& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(struct Struct1 & p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Struct1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToConstStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const struct Struct1 &").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(const struct Struct1& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const struct Struct1 & p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Struct1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                              Simple Right-Value Reference Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a right-value reference to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#%s &&", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(%s&& p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                  "void function1(%s && p)\n{\n"
                  "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                  "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a right-value reference to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        SimpleString typeKey = StringFromFormat( "#const %s &&", typeData.mockedType.c_str() );
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", typeKey.asCharString()).andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "void function1(const %s&& p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                  "void function1(const %s && p)\n{\n"
                  "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                  "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a right-value reference to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(Class1 && p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToConstClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(const Class1&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const Class1 && p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a non-const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Class1<short> &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short>&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(Class1<short> && p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1<short>\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const template class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToConstTemplateClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Class1<short> &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(const Class1<short>&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const Class1<short> && p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a non-const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#struct Struct1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(struct Struct1&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(struct Struct1 && p)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Struct1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const struct parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnRVReferenceToConstStructParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const struct Struct1 &&").andReturnValue((const void*)0);

    SimpleString testHeader =
            "struct Struct1 { int member1[100]; };\n"
            "void function1(const struct Struct1&& p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
            "void function1(const struct Struct1 && p)\n{\n"
            "    mock().actualCall(\"function1\").withParameterOfType(\"Struct1\", \"p\", &p);\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                 Pointer to Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to a typedef for a void type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTypedefForVoidParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef void Type1;\n"
            "void function1(Type1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a void type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstPointerToTypedefForVoidParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *const").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef void Type1;\n"
            "void function1(Type1* const p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 *const p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTypedefForPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(Type1* p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a pointer to a typedef for a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTypedefForConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s Type1;\n"
                "void function1(Type1* p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a pointer to a const typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstTypedefForPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type1 *").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(const Type1* p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstPointerToTypedefForPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *const").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(Type1* const p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 *const p)\n{\n"
                      "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a pointer to a typedef for a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTypedefForClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "void function1(Type1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a typedef for a const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToTypedefForConstClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1 Type1;"
            "void function1(Type1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const typedef for a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstTypedefForClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type1 *").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "void function1(const Type1* p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstPointerToTypedefForClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::UseUnderlyingTypedefType").onObject(config).andReturnValue(false);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1 *const").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "void function1(Type1* const p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 *const p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                 Typedef for Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for a pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a const pointer to a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForConstPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* const Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a const typedef for a pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type1").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "void function1(const Type1 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const typedef for a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "void function1(const Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a typedef for a pointer to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForPointerToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* Type1;"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a pointer to a const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForPointerToConstClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1* Type1;"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a const pointer to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForConstPointerToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* const Type1;"
            "void function1(Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const typedef for a pointer to a non-const class parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForPointerToClassParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type1").andReturnValue((const void*)0);

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* Type1;"
            "void function1(const Type1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                 Reference to Typedef Parameters
//*************************************************************************************************

//*************************************************************************************************
//                                 Typedef for Reference Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for a reference to a non-const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForReferenceToPrimitiveTypeParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config* config = GetMockConfig();
            mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
            mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type1").andReturnValue((const void*)0);

            SimpleString testHeader = StringFromFormat(
                    "typedef %s%s Type1;\n"
                    "void function1(Type1 p);",
                    typeData.originalType.c_str(), referenceType.c_str() );

            // Exercise
            std::vector<std::string> results;
            unsigned int functionCount = ParseHeader( testHeader, *config, results );

            // Verify
            mock().checkExpectations();
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                          "    mock().actualCall(\"function1\").withPointerParameter(\"p\", &p);\n"
                          "}\n", results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
            mock().clear();
        }
    }
}
#endif

//*************************************************************************************************
//                                 Pointer to Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to a pointer parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToPointerParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#param").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#int **").andReturnValue((const void*)0);

    SimpleString testHeader = "void function1(signed int* * param);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(int ** param)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"param\", param);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const pointer parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnPointerToConstPointerParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "functionX#x").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const struct ExternStruct **").andReturnValue((const void*)0);

    SimpleString testHeader = "void functionX(const struct ExternStruct* *x);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void functionX(const struct ExternStruct ** x)\n{\n"
                  "    mock().actualCall(\"functionX\").withPointerParameter(\"x\", x);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to a pointer parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstPointerToPointerParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "functionY#arg").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#short *const *").andReturnValue((const void*)0);

    SimpleString testHeader = "void functionY(short* const * arg);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void functionY(short *const * arg)\n{\n"
                  "    mock().actualCall(\"functionY\").withConstPointerParameter(\"arg\", arg);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                           Left-Value Reference to Pointer Parameters
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a left-value reference to a pointer parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToPointerParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#i").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#double *&").andReturnValue((const void*)0);

    SimpleString testHeader = "void function1(double* &i);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(double *& i)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"i\", &i);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const pointer parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnLVReferenceToConstPointerParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#j").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#bool *const &").andReturnValue((const void*)0);

    SimpleString testHeader = "void function1(bool* const &j);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(bool *const & j)\n{\n"
                  "    mock().actualCall(\"function1\").withConstPointerParameter(\"j\", &j);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                Typedef for Basic Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a double typedef for a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTypedefForPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(Type2 p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                          Typedef for Typedef for Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTypedefForPointerToPrimitiveTypePointerParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForConstTypedefForPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a pointer to a primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForTypedefForPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(const Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTypedefForPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForConstTypedefForPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForTypedefForPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type2").andReturnValue((const void*)0);

        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(const Type2 p);",
                typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type2 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const typedef for a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForTypedefForStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "void function1(Type2 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnTypedefForConstTypedefForStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef const Type1 Type2;\n"
            "void function1(Type2 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a string parameter and without return value.
 */
TEST_EX( TEST_GROUP_NAME, VoidReturnConstTypedefForTypedefForStringParameter )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const Type2").andReturnValue((const void*)0);

    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "void function1(const Type2 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//
//                                           COMBINATION
//
//*************************************************************************************************

/*
 * Check mock generation of a function with return and multiple parameters.
 */
TEST_EX( TEST_GROUP_NAME, ReturnAndMultipleParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p1").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p2").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p3").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p4").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@unsigned long").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const int *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const char *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#signed char *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#short").andReturnValue((const void*)0);

    SimpleString testHeader = "unsigned long function1(const signed int* p1, const char* p2, signed char* p3, short p4);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "unsigned long function1(const int * p1, const char * p2, signed char * p3, short p4)\n{\n"
                  "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1).withStringParameter(\"p2\", p2)"
                       ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with return and multiple unnamed parameters.
 */
TEST_EX( TEST_GROUP_NAME, MultipleUnnamedParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#_unnamedArg0").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#_unnamedArg1").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p3").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#_unnamedArg3").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@unsigned long").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const int *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const char *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#signed char *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#short").andReturnValue((const void*)0);

    SimpleString testHeader = "unsigned long function1(const signed int*, const char*, signed char* p3, short);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "unsigned long function1(const int * _unnamedArg0, const char * _unnamedArg1, signed char * p3, short _unnamedArg3)\n{\n"
                  "    return mock().actualCall(\"function1\").withConstPointerParameter(\"_unnamedArg0\", _unnamedArg0).withStringParameter(\"_unnamedArg1\", _unnamedArg1)"
                       ".withOutputParameter(\"p3\", p3).withIntParameter(\"_unnamedArg3\", _unnamedArg3).returnUnsignedLongIntValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with parameter override.
 */
TEST_EX( TEST_GROUP_NAME, ParameterOverride )
{
    struct ArgumentOverrideData
    {
        MockedType mockedType;
        std::string cpputestFunctionType;
        std::string argExprFront;
        std::string argExprBack;
    };

    const std::vector< struct ArgumentOverrideData > overrideOptions
    {
        { MockedType::Bool, "Bool", "(*", "!=0)" },
        { MockedType::Int, "Int", "*", "" },
        { MockedType::UnsignedInt, "UnsignedInt", "(unsigned)*", "" },
        { MockedType::Long, "LongInt", "*", "" },
        { MockedType::UnsignedLong, "UnsignedLongInt", "(unsigned)*", "" },
        { MockedType::Double, "Double", "*", "" },
        { MockedType::String, "String", "StringFromFormat(\"%c\", *", ").asCharString()" },
        { MockedType::Pointer, "Pointer", "", "" },
        { MockedType::ConstPointer, "ConstPointer", "", "" },
        { MockedType::Output, "Output", "", "" },
    };

    for( auto overrideOption : overrideOptions )
    {
        // Prepare
        mock().installCopier( "std::string", stdStringCopier );

        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p1").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p2").andReturnValue(override);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p3").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p4").andReturnValue((const void*)0);

        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@unsigned long").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const int *").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#signed char *").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#short").andReturnValue((const void*)0);

        mock().expectOneCall("Config::OverrideSpec::GetType").onObject((void*)override).andReturnValue((int)overrideOption.mockedType);
        mock().expectOneCall("Config::OverrideSpec::GetExprModFront").onObject((void*)override).andReturnValue((const void*)&overrideOption.argExprFront);
        mock().expectOneCall("Config::OverrideSpec::GetExprModBack").onObject((void*)override).andReturnValue((const void*)&overrideOption.argExprBack);

        SimpleString testHeader = "unsigned long function1(const signed int* p1, char* p2, signed char* p3, short p4);\n";

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "unsigned long function1(const int * p1, char * p2, signed char * p3, short p4)\n{\n"
            "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
                 ".with%sParameter(\"p2\", %sp2%s)"
                 ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
            "}\n", overrideOption.cpputestFunctionType.c_str(), overrideOption.argExprFront.c_str(), overrideOption.argExprBack.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with parameter override.
 */
TEST_EX( TEST_GROUP_NAME,ReturnOverride )
{
    struct ReturnOverrideData
    {
        MockedType mockedType;
        std::string cpputestFunctionType;
        std::string argExprFront;
        std::string argExprBack;
    };

    const std::vector< struct ReturnOverrideData > overrideOptions
    {
        { MockedType::Bool, "Bool", "( ", " ? 123 : 0 )" },
        { MockedType::Int, "Int", "(unsigned long) ", "" },
        { MockedType::UnsignedInt, "UnsignedInt", "", "" },
        { MockedType::Long, "LongInt", "(unsigned long) ", "" },
        { MockedType::UnsignedLong, "UnsignedLongInt", "(", " + 1)" },
        { MockedType::Double, "Double", "(unsigned long) ", "" },
        { MockedType::String, "String", "(unsigned long) ", "[0]" },
        { MockedType::Pointer, "Pointer", "(* (unsigned long *) ", ")" },
        { MockedType::ConstPointer, "ConstPointer", "((unsigned long *) ", ")[0]" },
    };

    for( auto overrideOption : overrideOptions )
    {
        // Prepare
        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(199);
        const std::string overrideArgExprFront = "(*(unsigned long*)";
        const std::string overrideArgExprBack = ")+1";
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue(override);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p1").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p2").andReturnValue((const void*)0);

        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const int *").andReturnValue((const void*)0);
        mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const char *").andReturnValue((const void*)0);

        mock().expectOneCall("Config::OverrideSpec::GetType").onObject((void*)override).andReturnValue((int)overrideOption.mockedType);
        mock().expectOneCall("Config::OverrideSpec::GetExprModFront").onObject((void*)override).andReturnValue((const void*)&overrideOption.argExprFront);
        mock().expectOneCall("Config::OverrideSpec::GetExprModBack").onObject((void*)override).andReturnValue((const void*)&overrideOption.argExprBack);

        SimpleString testHeader = "unsigned long function1(const signed int* p1, const char* p2);";

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "unsigned long function1(const int * p1, const char * p2)\n{\n"
                "    return %smock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1).withStringParameter(\"p2\", p2)"
                     ".return%sValue()%s;\n"
                "}\n", overrideOption.argExprFront.c_str(), overrideOption.cpputestFunctionType.c_str(), overrideOption.argExprBack.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with parameter override that skips a parameter.
 */
TEST_EX( TEST_GROUP_NAME, ParameterOverride_Skip )
{
    // Prepare
    mock().installCopier( "std::string", stdStringCopier );

    Config* config = GetMockConfig();
    const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1@").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p1").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p2").andReturnValue(override);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p3").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetParameterOverride").onObject(config).withStringParameter("key", "function1#p4").andReturnValue((const void*)0);

    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "@unsigned long").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#const int *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#signed char *").andReturnValue((const void*)0);
    mock().expectOneCall("Config::GetTypeOverride").onObject(config).withStringParameter("key", "#short").andReturnValue((const void*)0);

    mock().expectOneCall("Config::OverrideSpec::GetType").onObject((void*)override).andReturnValue((int)MockedType::Skip);

    SimpleString testHeader = "unsigned long function1(const signed int* p1, const char* p2, signed char* p3, short p4);\n";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "unsigned long function1(const int * p1, const char *, signed char * p3, short p4)\n{\n"
                  "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
                       ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckExpectationCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#endif
