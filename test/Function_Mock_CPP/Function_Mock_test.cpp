/**
 * @file
 * @brief      Unit tests for the "Function" class (mock generation)
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>
#include <functional>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <vector>
#include <string>

#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"

#include "Function.hpp"

#include "Config_expect.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

#ifdef INTERPRET_C
#define TEST_GROUP_EX(x) TEST_GROUP_BASE(Function_Mock_C_ ## x, FunctionBase)
#define TEST_EX(x, y) TEST(Function_Mock_C_ ## x, y)
#else
#define TEST_GROUP_EX(x) TEST_GROUP_BASE(Function_Mock_CPP_ ## x, FunctionBase)
#define TEST_EX(x, y) TEST(Function_Mock_CPP_ ## x, y)
#endif

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

TEST_BASE( FunctionBase )
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

    unsigned int ParseHeader( const SimpleString &testHeader, const Config &config, std::vector<std::string> &results )
    {
        unsigned int functionCount = 0;

        ClangParseHelper::ParseHeader( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function;
            if( function.Parse( cursor, config ) )
            {
                results.push_back( function.GenerateMock() );
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

TEST_GROUP_EX( Generic )
{    
};

TEST_GROUP_EX( Parameter )
{    
};

TEST_GROUP_EX( Return )
{    
};

TEST_GROUP_EX( Combination )
{    
};

TEST_GROUP_EX( TypeOverrides )
{    
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
 * Check that a function with definition is not mocked.
 */
TEST_EX( Generic, WithDefinition )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function1() {}";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a function declared twice is mocked just once.
 */
TEST_EX( Generic, DoubleDeclaration )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "void function1();\n"
            "void function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 2, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1()\n{\n"
                  "    mock().actualCall(\"function1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check that a function inside a namespace is mocked properly.
 */
TEST_EX( Generic, FunctionWithinNamespace )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader =
            "namespace ns1 {\n"
            "void function1();\n"
            "}";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void ns1::function1()\n{\n"
                  "    mock().actualCall(\"ns1::function1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters nor return value.
 */
TEST_EX( Generic, NoReturnNoParameters )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function1();";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1()\n{\n"
                  "    mock().actualCall(\"function1\");\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with return and an explicit void parameter.
 */
TEST_EX( Generic, NoReturnExplicitVoidParameter )
{
    // Prepare
    Config* config = GetMockConfig();

    SimpleString testHeader = "void function(void);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function()\n{\n"
                  "    mock().actualCall(\"function\");\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//
//                                DIFFERENT RETURNS / NO PARAMETERS
//
//*************************************************************************************************

//*************************************************************************************************
//                                        Simple Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a primitive type value.
 */
TEST_EX( Return, PrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@%s", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "%s function1();", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult;
        if( typeData.casted )
        {
            expectedResult = StringFromFormat(
                    "%s function1()\n{\n"
                    "    return static_cast<%s>(mock().actualCall(\"function1\").return%sValue());\n"
                    "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        }
        else
        {
            expectedResult = StringFromFormat(
                    "%s function1()\n{\n"
                    "    return mock().actualCall(\"function1\").return%sValue();\n"
                    "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        }
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning an enum value.
 */
TEST_EX( Return, Enum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@" ENUM_TAG "Enum1", nullptr );

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
                  "    return static_cast<" ENUM_TAG "Enum1>(mock().actualCall(\"function1\").returnIntValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a scoped enum value.
 */
TEST_EX( Return, ScopedEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Enum1", nullptr );

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
                  "    return static_cast<Enum1>(mock().actualCall(\"function1\").returnIntValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a class.
 */
TEST_EX( Return, Class )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1", nullptr );

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
                  "    return *static_cast<const Class1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a template class.
 */
TEST_EX( Return, TemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1<int>", nullptr );

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
                  "    return *static_cast<const Class1<int>*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a struct.
 */
TEST_EX( Return, Struct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@" STRUCT_TAG "Struct1", nullptr );

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
                  "    return *static_cast<const " STRUCT_TAG "Struct1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                     Simple Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a typedef for a primitive type.
 */
TEST_EX( Return, TypedefForPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                "    return static_cast<Type1>(mock().actualCall(\"function1\").return%sValue());\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for an enum.
 */
TEST_EX( Return, TypedefForEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnIntValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a typedef for a scoped enum.
 */
TEST_EX( Return, TypedefForScopedEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnIntValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a class.
 */
TEST_EX( Return, TypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return *static_cast<const Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a template class.
 */
TEST_EX( Return, TypedefForTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return *static_cast<const Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a typedef for a struct.
 */
TEST_EX( Return, TypedefForStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return *static_cast<const Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                     Simple Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const void value.
 */
TEST_EX( Return, PointerToVoid )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@void *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const void value.
 */
TEST_EX( Return, PointerToConstVoid )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const void *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const primitive type.
 */
TEST_EX( Return, PointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@%s *", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return static_cast<%s*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const primitive type.
 */
TEST_EX( Return, PointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@const %s *", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return static_cast<const %s*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a string.
 */
TEST_EX( Return, String )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const char *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const class.
 */
TEST_EX( Return, PointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1 *", nullptr );

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
                  "    return static_cast<Class1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const class.
 */
TEST_EX( Return, PointerToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1 *", nullptr );

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
                  "    return static_cast<const Class1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const template class.
 */
TEST_EX( Return, PointerToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1<int> *", nullptr );

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
                  "    return static_cast<Class1<int>*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const template class.
 */
TEST_EX( Return, PointerToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1<char> *", nullptr );

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
                  "    return static_cast<const Class1<char>*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const struct.
 */
TEST_EX( Return, PointerToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@struct Struct1 *", nullptr );

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
                  "    return static_cast<struct Struct1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const struct.
 */
TEST_EX( Return, PointerToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const struct Struct1 *", nullptr );

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
                  "    return static_cast<const struct Struct1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                     Pointer to Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a non-const pointer to a pointer.
 */
TEST_EX( Return, PointerToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "@%s **", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "%s* *function1();", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "%s ** function1()\n{\n"
            "    return static_cast<%s **>(mock().actualCall(\"function1\").returnPointerValue());\n"
            "}\n", type.c_str(), type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a pointer.
 */
TEST_EX( Return, ConstPointerToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "@%s *const *", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "%s* const *function1();", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "%s *const * function1()\n{\n"
            "    return static_cast<%s *const*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
            "}\n", type.c_str(), type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                               Simple Left-Value Reference Return
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const primitive type.
 */
TEST_EX( Return, LVReferenceToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@%s &", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return *static_cast<%s*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const primitive type.
 */
TEST_EX( Return, LVReferenceToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@const %s &", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return *static_cast<const %s*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const class.
 */
TEST_EX( Return, LVReferenceToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1 &", nullptr );

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
            "    return *static_cast<Class1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const class.
 */
TEST_EX( Return, LVReferenceToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1 &", nullptr );

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
            "    return *static_cast<const Class1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const template class.
 */
TEST_EX( Return, LVReferenceToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1<int> &", nullptr );

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
            "    return *static_cast<Class1<int>*>(mock().actualCall(\"function1\").returnPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const template class.
 */
TEST_EX( Return, LVReferenceToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1<int> &", nullptr );

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
            "    return *static_cast<const Class1<int>*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const struct.
 */
TEST_EX( Return, LVReferenceToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@struct Struct1 &", nullptr );

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
            "    return *static_cast<struct Struct1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const struct.
 */
TEST_EX( Return, LVReferenceToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const struct Struct1 &", nullptr );

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
            "    return *static_cast<const struct Struct1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                             Left-Value Reference to Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a non-const pointer.
 */
TEST_EX( Return, LVReferenceToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "@%s *&", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "%s* &function1();", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "%s *& function1()\n{\n"
            "    return *static_cast<%s **>(mock().actualCall(\"function1\").returnPointerValue());\n"
            "}\n", type.c_str(), type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a left-value reference to a const pointer.
 */
TEST_EX( Return, LVReferenceToConstPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "@%s *const &", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "%s* const &function1();", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "%s *const & function1()\n{\n"
            "    return *static_cast<%s *const*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
            "}\n", type.c_str(), type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                                   Right-Value Reference Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const primitive type.
 */
TEST_EX( Return, RVReferenceToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@%s &&", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return std::move(*static_cast<%s*>(mock().actualCall(\"function1\").returnPointerValue()));\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const primitive type.
 */
TEST_EX( Return, RVReferenceToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        SimpleString typeKey = StringFromFormat( "@const %s &&", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
                "    return std::move(*static_cast<const %s*>(mock().actualCall(\"function1\").returnConstPointerValue()));\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const class.
 */
TEST_EX( Return, RVReferenceToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1 &&", nullptr );

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
            "    return std::move(*static_cast<Class1*>(mock().actualCall(\"function1\").returnPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const class.
 */
TEST_EX( Return, RVReferenceToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1 &&", nullptr );

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
            "    return std::move(*static_cast<const Class1*>(mock().actualCall(\"function1\").returnConstPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const template class.
 */
TEST_EX( Return, RVReferenceToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Class1<int> &&", nullptr );

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
            "    return std::move(*static_cast<Class1<int>*>(mock().actualCall(\"function1\").returnPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const template class.
 */
TEST_EX( Return, RVReferenceToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Class1<int> &&", nullptr );

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
            "    return std::move(*static_cast<const Class1<int>*>(mock().actualCall(\"function1\").returnConstPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a non-const struct.
 */
TEST_EX( Return, RVReferenceToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@struct Struct1 &&", nullptr );

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
            "    return std::move(*static_cast<struct Struct1*>(mock().actualCall(\"function1\").returnPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a right-value reference to a const struct.
 */
TEST_EX( Return, RVReferenceToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const struct Struct1 &&", nullptr );

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
            "    return std::move(*static_cast<const struct Struct1*>(mock().actualCall(\"function1\").returnConstPointerValue()));\n"
            "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                   Pointer to Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a non-const primitive type.
 */
TEST_EX( Return, PointerToTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1 *", nullptr );

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
                      "    return static_cast<Type1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a const primitive type.
 */
TEST_EX( Return, PointerToTypedefForConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1 *", nullptr );

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
                      "    return static_cast<Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const typedef for a primitive type.
 */
TEST_EX( Return, PointerToConstTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@const Type1 *", nullptr );

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
                      "    return static_cast<const Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a typedef for a primitive type.
 */
TEST_EX( Return, ConstPointerToTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1 *const", nullptr );

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
                      "    return static_cast<Type1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a pointer to a typedef for a class.
 */
TEST_EX( Return, PointerToTypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1 *", nullptr );

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
                  "    return static_cast<Type1*>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const typedef for a class.
 */
TEST_EX( Return, PointerToTypedefForConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1 *", nullptr );

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
                  "    return static_cast<Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const pointer to a typedef for a class.
 */
TEST_EX( Return, PointerToConstTypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Type1 *", nullptr );

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
                  "    return static_cast<const Type1*>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                   Typedef for Pointer Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a non-const primitive type.
 */
TEST_EX( Return, TypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                      "    return static_cast<Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a const primitive type.
 */
TEST_EX( Return, TypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                      "    return static_cast<Type1>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a const pointer to a non-const primitive type.
 */
TEST_EX( Return, TypedefForConstPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                      "    return static_cast<Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a pointer to a non-const primitive type.
 */
TEST_EX( Return, ConstTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@const Type1", nullptr );

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
                      "    return static_cast<const Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a string.
 */
TEST_EX( Return, TypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnStringValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a string.
 */
TEST_EX( Return, ConstTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Type1", nullptr );

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
                  "    return static_cast<const Type1>(mock().actualCall(\"function1\").returnStringValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a non-const class.
 */
TEST_EX( Return, TypedefForPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to a const class.
 */
TEST_EX( Return, TypedefForPointerToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a const pointer to a non-const class.
 */
TEST_EX( Return, TypedefForConstPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                  "    return static_cast<Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a pointer to a non-const class.
 */
TEST_EX( Return, ConstTypedefForPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Type1", nullptr );

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
                  "    return static_cast<const Type1>(mock().actualCall(\"function1\").returnPointerValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

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
TEST_EX( Return, TypedefForLVReferenceToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                "    return static_cast<Type1>(*static_cast<%s*>(mock().actualCall(\"function1\").returnPointerValue()));\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a right-value reference to a non-const primitive type.
 */
TEST_EX( Return, TypedefForRVReferenceToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type1", nullptr );

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
                "    return static_cast<Type1>(std::move(*static_cast<%s*>(mock().actualCall(\"function1\").returnPointerValue())));\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}
#endif

//*************************************************************************************************
//                                Typedef for Simple Typedef Return
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters and returning a double typedef for a primitive type.
 */
TEST_EX( Return, TypedefForTypedefForPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                "    return static_cast<Type2>(mock().actualCall(\"function1\").return%sValue());\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

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
TEST_EX( Return, TypedefForTypedefForPointerToPrimitiveTypePointer )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                      "    return static_cast<Type2>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a primitive type.
 */
TEST_EX( Return, TypedefForConstTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                      "    return static_cast<Type2>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a primitive type.
 */
TEST_EX( Return, ConstTypedefForTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@const Type2", nullptr );

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
                      "    return static_cast<const Type2>(mock().actualCall(\"function1\").returnPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST_EX( Return, TypedefForTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                      "    return static_cast<Type2>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST_EX( Return, TypedefForConstTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                      "    return static_cast<Type2>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a const primitive type.
 */
TEST_EX( Return, ConstTypedefForTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "@const Type2", nullptr );

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
                      "    return static_cast<const Type2>(mock().actualCall(\"function1\").returnConstPointerValue());\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const typedef for a string.
 */
TEST_EX( Return, TypedefForTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                  "    return static_cast<Type2>(mock().actualCall(\"function1\").returnStringValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a string.
 */
TEST_EX( Return, TypedefForConstTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@Type2", nullptr );

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
                  "    return static_cast<Type2>(mock().actualCall(\"function1\").returnStringValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a string.
 */
TEST_EX( Return, ConstTypedefForTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "@const Type2", nullptr );

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
                  "    return static_cast<const Type2>(mock().actualCall(\"function1\").returnStringValue());\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

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
TEST_EX( Parameter, PrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#%s", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s p);", typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(%s p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with an enum parameter and without return value.
 */
TEST_EX( Parameter, Enum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#" ENUM_TAG "Enum1", nullptr );

    SimpleString testHeader =
            "enum Enum1 { X, Y, Z };\n"
            "void function1(" ENUM_TAG "Enum1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(" ENUM_TAG "Enum1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a scoped enum parameter and without return value.
 */
TEST_EX( Parameter, ScopedEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Enum1", nullptr );

    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z };\n"
            "void function1(Enum1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Enum1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a class parameter and without return value.
 */
TEST_EX( Parameter, Class )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1", nullptr );

    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1 p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a template class parameter and without return value.
 */
TEST_EX( Parameter, TemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1<short>", nullptr );

    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short> p);";

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1<short> p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function with a struct parameter and without return value.
 */
TEST_EX( Parameter, Struct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#" STRUCT_TAG "Struct1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                  Simple Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for an enum parameter and without return value.
 */
TEST_EX( Parameter, TypedefForEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a typedef for a scoped enum parameter and without return value.
 */
TEST_EX( Parameter, TypedefForScopedEnum )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForClass_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, true );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a template class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a template class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForTemplateClass_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, true );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function with a typedef for a struct parameter and without return value.
 */
TEST_EX( Parameter, TypedefForStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a struct parameter and without return value.
 */
TEST_EX( Parameter, TypedefForStruct_UseUnderlyingType )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, true );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                   Simple Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to non-const void parameter and without return value.
 */
TEST_EX( Parameter, PointerToVoid )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#void *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, PointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#%s *", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#const %s *", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a string parameter and without return value.
 */
TEST_EX( Parameter, String )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#const char *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a pointer to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, PointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const class parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a non-const template class parameter and without return value.
 */
TEST_EX( Parameter, PointerToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1<short> *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const template class parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1<short> *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

/*
 * Check mock generation of a function with a pointer to a non-const struct parameter and without return value.
 */
TEST_EX( Parameter, PointerToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#struct Struct1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const struct parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const struct Struct1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                 Pointer to Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to a pointer parameter and without return value.
 */
TEST_EX( Parameter, PointerToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "#%s **", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#param", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s* * param);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "void function1(%s ** param)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameter(\"param\", param);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a const pointer to a pointer parameter and without return value.
 */
TEST_EX( Parameter, ConstPointerToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "#%s *const *", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "functionY#arg", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void functionY(%s* const * arg);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "void functionY(%s *const * arg)\n{\n"
            "    mock().actualCall(\"functionY\").withConstPointerParameter(\"arg\", arg);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                              Simple Array Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with an array of non-const types parameter and without return value.
 */
TEST_EX( Parameter, ArrayOfTypes )
{
    auto testedTypes = primitivePointedTypesWithString;

    testedTypes.insert( testedTypes.end(),
    { 
#ifndef INTERPRET_C
        { "ScopedEnum1", "ScopedEnum1" },
        { "Class1", "Class1" },
        { "TClass1<short>", "TClass1<short>" },
        { "Enum2", "Enum2" },
#else
        { "enum Enum2", "enum Enum2" },
#endif
        { "struct Struct1", "struct Struct1" }
    } );

    for( auto typeData : testedTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#%s []", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat(
#ifndef INTERPRET_C
            "enum class ScopedEnum1 { A, B, C };\n"
            "class Class1 { int member1[100]; };\n"
            "template<class T> class TClass1 { T member1[100]; };\n"
#endif
            "enum Enum2 { X, Y, Z };\n"
            "struct Struct1 { int member1[100]; };\n"
            "void function1(%s p[]);",
            typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "void function1(%s p[])\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
            "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with an array of const types parameter and without return value.
 */
TEST_EX( Parameter, ArrayOfConstTypes )
{
    auto testedTypes = primitivePointedTypesWithString;

    testedTypes.insert( testedTypes.end(),
    { 
#ifndef INTERPRET_C
        { "ScopedEnum1", "ScopedEnum1" },
        { "Class1", "Class1" },
        { "TClass1<short>", "TClass1<short>" },
        { "Enum2", "Enum2" },
#else
        { "enum Enum2", "enum Enum2" },
#endif
        { "struct Struct1", "struct Struct1" }
    } );

    for( auto typeData : testedTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#const %s []", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat(
#ifndef INTERPRET_C
            "enum class ScopedEnum1 { A, B, C };\n"
            "class Class1 { int member1[100]; };\n"
            "template<class T> class TClass1 { T member1[100]; };\n"
#endif
            "enum Enum2 { X, Y, Z };\n"
            "struct Struct1 { int member1[100]; };\n"
            "void function1(const %s p[]);",
            typeData.originalType.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "void function1(const %s p[])\n{\n"
            "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
            "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                           Array of Pointers Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with an array of pointer parameters and without return value.
 */
TEST_EX( Parameter, ArrayOfPointers )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int" };

    for( auto type : types )
    {
        // Prepare
        auto typeOverride = StringFromFormat( "#%s *[]", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#i", nullptr );
        expect::Config$::GetTypeOverride( config, typeOverride.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s* i[]);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "void function1(%s * i[])\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameter(\"i\", i);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with an array of const pointers parameter and without return value.
 */
TEST_EX( Parameter, ArrayOfConstPointers )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int" };

    for( auto type : types )
    {
        // Prepare
        auto typeOverride = StringFromFormat( "#%s *const []", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#j", nullptr );
        expect::Config$::GetTypeOverride( config, typeOverride.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s* const j[]);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat( 
            "void function1(%s *const j[])\n{\n"
            "    mock().actualCall(\"function1\").withConstPointerParameter(\"j\", j);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                              Simple Left-Value Reference Parameters
//*************************************************************************************************

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a left-value reference to a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#%s &", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a left-value reference to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#const %s &", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a left-value reference to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1 &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const class parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1 &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a non-const template class parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1<short> &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const template class parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1<short> &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a non-const struct parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#struct Struct1 &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a left-value reference to a const struct parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const struct Struct1 &", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                           Left-Value Reference to Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a left-value reference to a pointer parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "#%s *&", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#i", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s* &i);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "void function1(%s *& i)\n{\n"
            "    mock().actualCall(\"function1\").withOutputParameter(\"i\", &i);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a left-value reference to a const pointer parameter and without return value.
 */
TEST_EX( Parameter, LVReferenceToConstPointer )
{
    const std::vector<std::string> types = { "void", "const void", "int", "const int", "struct ExternStruct", "const struct ExternStruct" };

    for( auto type : types )
    {
        // Prepare
        SimpleString typeKey = StringFromFormat( "#%s *const &", type.c_str() );

        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#j", nullptr );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

        SimpleString testHeader = StringFromFormat( "void function1(%s* const &j);", type.c_str() );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( StringFromFormat(
            "void function1(%s *const & j)\n{\n"
            "    mock().actualCall(\"function1\").withConstPointerParameter(\"j\", &j);\n"
            "}\n", type.c_str() ).asCharString(),
            results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

//*************************************************************************************************
//                              Simple Right-Value Reference Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a right-value reference to a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#%s &&", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a right-value reference to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToConstPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        SimpleString typeKey = StringFromFormat( "#const %s &&", typeData.mockedType.c_str() );
        expect::Config$::GetTypeOverride( config, typeKey.asCharString(), nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a right-value reference to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1 &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const class parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1 &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a non-const template class parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Class1<short> &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const template class parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToConstTemplateClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Class1<short> &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a non-const struct parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#struct Struct1 &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a right-value reference to a const struct parameter and without return value.
 */
TEST_EX( Parameter, RVReferenceToConstStruct )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const struct Struct1 &&", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                 Pointer to Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a pointer to a typedef for a void type parameter and without return value.
 */
TEST_EX( Parameter, PointerToTypedefForVoid )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a void type parameter and without return value.
 */
TEST_EX( Parameter, ConstPointerToTypedefForVoid )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1 *const", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, PointerToTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1 *", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a pointer to a typedef for a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, PointerToTypedefForConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1 *", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a pointer to a const typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#const Type1 *", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, ConstPointerToTypedefForPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1 *const", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a pointer to a typedef for a class parameter and without return value.
 */
TEST_EX( Parameter, PointerToTypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a typedef for a const class parameter and without return value.
 */
TEST_EX( Parameter, PointerToTypedefForConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to a const typedef for a class parameter and without return value.
 */
TEST_EX( Parameter, PointerToConstTypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#const Type1 *", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to a typedef for a class parameter and without return value.
 */
TEST_EX( Parameter, ConstPointerToTypedefForClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::UseUnderlyingTypedefType( config, false );
    expect::Config$::GetTypeOverride( config, "#Type1 *const", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
#endif

//*************************************************************************************************
//                                 Typedef for Pointer Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for a pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a const pointer to a primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForConstPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a const typedef for a pointer to a non-const primitive type parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#const Type1", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for a string parameter and without return value.
 */
TEST_EX( Parameter, TypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const typedef for a string parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#const Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

#ifndef INTERPRET_C
/*
 * Check mock generation of a function with a typedef for a pointer to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a pointer to a const class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForPointerToConstClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a const pointer to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, TypedefForConstPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const typedef for a pointer to a non-const class parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForPointerToClass )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#const Type1", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                 Typedef for Array Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a typedef for an array of non-const types parameter and without return value.
 */
TEST_EX( Parameter, TypedefForArrayOfTypes )
{
    auto testedTypes = primitivePointedTypesWithString;

    testedTypes.insert( testedTypes.end(),
    { 
#ifndef INTERPRET_C
        { "ScopedEnum1", "ScopedEnum1" },
        { "Class1", "Class1" },
        { "TClass1<short>", "TClass1<short>" },
        { "Enum2", "Enum2" },
#else
        { "enum Enum2", "enum Enum2" },
#endif
        { "struct Struct1", "struct Struct1" }
    } );

    for( auto typeData : testedTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

        SimpleString testHeader = StringFromFormat(
#ifndef INTERPRET_C
            "enum class ScopedEnum1 { A, B, C };\n"
            "class Class1 { int member1[100]; };\n"
            "template<class T> class TClass1 { T member1[100]; };\n"
#endif
            "enum Enum2 { X, Y, Z };\n"
            "struct Struct1 { int member1[100]; };\n"
            "typedef %s Type1[];\n"
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
                      "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a typedef for an array of const types parameter and without return value.
 */
TEST_EX( Parameter, TypedefForArrayOfConstTypes )
{
    auto testedTypes = primitivePointedTypesWithString;

    testedTypes.insert( testedTypes.end(),
    { 
#ifndef INTERPRET_C
        { "ScopedEnum1", "ScopedEnum1" },
        { "Class1", "Class1" },
        { "TClass1<short>", "TClass1<short>" },
        { "Enum2", "Enum2" },
#else
        { "enum Enum2", "enum Enum2" },
#endif
        { "struct Struct1", "struct Struct1" }
    } );

    for( auto typeData : testedTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

        SimpleString testHeader = StringFromFormat(
#ifndef INTERPRET_C
            "enum class ScopedEnum1 { A, B, C };\n"
            "class Class1 { int member1[100]; };\n"
            "template<class T> class TClass1 { T member1[100]; };\n"
#endif
            "enum Enum2 { X, Y, Z };\n"
            "struct Struct1 { int member1[100]; };\n"
            "typedef const %s Type1[];\n"
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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a const typedef for an array of types parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForArrayOfTypes )
{
    auto testedTypes = primitivePointedTypesWithString;

    testedTypes.insert( testedTypes.end(),
    { 
#ifndef INTERPRET_C
        { "ScopedEnum1", "ScopedEnum1" },
        { "Class1", "Class1" },
        { "TClass1<short>", "TClass1<short>" },
        { "Enum2", "Enum2" },
#else
        { "enum Enum2", "enum Enum2" },
#endif
        { "struct Struct1", "struct Struct1" }
    } );

    for( auto typeData : testedTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#const Type1", nullptr );

        SimpleString testHeader = StringFromFormat(
#ifndef INTERPRET_C
            "enum class ScopedEnum1 { A, B, C };\n"
            "class Class1 { int member1[100]; };\n"
            "template<class T> class TClass1 { T member1[100]; };\n"
#endif
            "enum Enum2 { X, Y, Z };\n"
            "struct Struct1 { int member1[100]; };\n"
            "typedef %s Type1[];\n"
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
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
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
TEST_EX( Parameter, TypedefForReferenceToPrimitiveType )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config* config = GetMockConfig();
            expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
            expect::Config$::GetTypeOverride( config, "#Type1", nullptr );

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
            CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
            mock().clear();
        }
    }
}
#endif

//*************************************************************************************************
//                                Typedef for Simple Typedef Parameters
//*************************************************************************************************

/*
 * Check mock generation of a function with a double typedef for a primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForTypedefForPrimitiveType )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

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
TEST_EX( Parameter, TypedefForTypedefForPointerToPrimitiveTypePointer )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForConstTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a pointer to a primitive type parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForTypedefForPointerToPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#const Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, TypedefForConstTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a pointer to a const primitive type parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForTypedefForPointerToConstPrimitiveType )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config* config = GetMockConfig();
        expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
        expect::Config$::GetTypeOverride( config, "#const Type2", nullptr );

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
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with a double non-const typedef for a string parameter and without return value.
 */
TEST_EX( Parameter, TypedefForTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a string parameter and without return value.
 */
TEST_EX( Parameter, TypedefForConstTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#Type2", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a string parameter and without return value.
 */
TEST_EX( Parameter, ConstTypedefForTypedefForString )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1#p", nullptr );
    expect::Config$::GetTypeOverride( config, "#const Type2", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

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
TEST_EX( Combination, ReturnAndMultipleParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p2", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );
    expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
    expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
    expect::Config$::GetTypeOverride( config, "#const char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#short", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with return and multiple unnamed parameters.
 */
TEST_EX( Combination, MultipleUnnamedParameters )
{
    // Prepare
    Config* config = GetMockConfig();
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#_unnamedArg0", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#_unnamedArg1", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#_unnamedArg3", nullptr );
    expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
    expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
    expect::Config$::GetTypeOverride( config, "#const char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#short", nullptr );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//
//                                         TYPE OVERRIDES
//
//*************************************************************************************************

/*
 * Check mock generation of a function with parameter override.
 */
TEST_EX( TypeOverrides, ParameterOverride_Simple )
{
    struct TestData
    {
        MockedType mockedType;
        std::string cpputestFunctionType;
        std::string argExprFront;
        std::string argExprBack;
        std::string expectedCallExpr;
    };

    const std::vector<struct TestData> testDataList
    {
        { MockedType::Bool, "Bool", "(", "->a != 0)", "(p2->a != 0)" },
        { MockedType::Int, "Int", "(*", ").a", "(*p2).a" },
        { MockedType::UnsignedInt, "UnsignedInt", "(unsigned)(", "->a)", "(unsigned)(p2->a)" },
        { MockedType::Long, "LongInt", "", "->a", "p2->a" },
        { MockedType::UnsignedLong, "UnsignedLongInt", "(unsigned)(", "->a)", "(unsigned)(p2->a)" },
        { MockedType::Double, "Double", "(*", ").a", "(*p2).a" },
        { MockedType::String, "String", "StringFromFormat(\"%d\", ", "->a).asCharString()", "StringFromFormat(\"%d\", p2->a).asCharString()" },
        { MockedType::Pointer, "Pointer", "", "", "p2" },
        { MockedType::ConstPointer, "ConstPointer", "", "", "p2" },
        { MockedType::Output, "Output", "(", ")", "(p2)" },
        { MockedType::InputPOD, "MemoryBuffer", "(", ")", "static_cast<const unsigned char *>(static_cast<const void *>((p2))), sizeof(*(p2))" },
        { MockedType::OutputPOD, "Output", "(", ")", "(p2)" },
    };

    SimpleString testHeader =
            "struct Struct1 { int a; };\n"
            "unsigned long function1(const signed int* p1, struct Struct1* p2, signed char* p3, short p4);\n";

    mock().installCopier( "std::string", stdStringCopier );

    for( auto testData : testDataList )
    {
        // Prepare
        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p2", override );
        expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

        expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
        expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
        expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
        expect::Config$::GetTypeOverride( config, "#short", nullptr );

        expect::Config$::OverrideSpec$::GetType( override, testData.mockedType );
        expect::Config$::OverrideSpec$::GetExprModFront( override, &testData.argExprFront );
        expect::Config$::OverrideSpec$::GetExprModBack( override, &testData.argExprBack );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "unsigned long function1(const int * p1, struct Struct1 * p2, signed char * p3, short p4)\n{\n"
            "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
                 ".with%sParameter(\"p2\", %s)"
                 ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
            "}\n", testData.cpputestFunctionType.c_str(), testData.expectedCallExpr.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}

/*
 * Check mock generation of a function with parameter override of type InputOfType.
 */
TEST_EX( TypeOverrides, ParameterOverride_InputOfType )
{
    // Prepare
    const std::string exposedTypeName = "Struct1";
    const std::string argExprFront = "&(";
    const std::string argExprBack = "->s)";
    const std::string expectationArgTypeName = "98432~#";

    SimpleString testHeader =
            "struct Struct1 { int a; };\n"
            "struct Struct2 { struct Struct1 s; };\n"
            "unsigned long function1(const signed int* p1, struct Struct2* p2, signed char* p3, short p4);\n";

    mock().installCopier( "std::string", stdStringCopier );

    Config* config = GetMockConfig();
    const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p2", override );
    expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

    expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
    expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
    expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#short", nullptr );

    expect::Config$::OverrideSpec$::GetType( override, MockedType::InputOfType );
    expect::Config$::OverrideSpec$::GetExprModFront( override, &argExprFront );
    expect::Config$::OverrideSpec$::GetExprModBack( override, &argExprBack );
    expect::Config$::OverrideSpec$::GetExposedTypeName( override, &exposedTypeName );
    expect::Config$::OverrideSpec$::GetExpectationArgTypeName( override, &expectationArgTypeName );

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
        "unsigned long function1(const int * p1, struct Struct2 * p2, signed char * p3, short p4)\n{\n"
        "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
             ".withParameterOfType(\"Struct1\", \"p2\", &(p2->s))"
             ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
        "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with parameter override of type OutputOfType.
 */
TEST_EX( TypeOverrides, ParameterOverride_OutputOfType )
{
    // Prepare
    const std::string exposedTypeName = "Struct1";
    const std::string argExprFront = "&(";
    const std::string argExprBack = "->s)";
    const std::string expectationArgTypeName = "98432~#";

    SimpleString testHeader =
            "struct Struct1 { int a; };\n"
            "struct Struct2 { struct Struct1 s; };\n"
            "unsigned long function1(const signed int* p1, struct Struct2* p2, signed char* p3, short p4);\n";

    mock().installCopier( "std::string", stdStringCopier );

    Config* config = GetMockConfig();
    const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p2", override );
    expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

    expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
    expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
    expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#short", nullptr );

    expect::Config$::OverrideSpec$::GetType( override, MockedType::OutputOfType );
    expect::Config$::OverrideSpec$::GetExprModFront( override, &argExprFront );
    expect::Config$::OverrideSpec$::GetExprModBack( override, &argExprBack );
    expect::Config$::OverrideSpec$::GetExposedTypeName( override, &exposedTypeName );
    expect::Config$::OverrideSpec$::GetExpectationArgTypeName( override, &expectationArgTypeName );

    // Exercise
    std::vector<std::string> results;
    unsigned int functionCount = ParseHeader( testHeader, *config, results );

    // Verify
    mock().checkExpectations();
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    SimpleString expectedResult =
        "unsigned long function1(const int * p1, struct Struct2 * p2, signed char * p3, short p4)\n{\n"
        "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
             ".withOutputParameterOfType(\"Struct1\", \"p2\", &(p2->s))"
             ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
        "}\n";
    STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with parameter override of type MemoryBuffer.
 */
TEST_EX( TypeOverrides, ParameterOverride_MemoryBuffer_FromPointer )
{
    struct TestData
    {
        bool hasPlaceholder;
        std::string sizeExprFront;
        std::string sizeExprBack;
        std::string expectedCallExpr;
    };

    const std::vector<struct TestData> testDataList
    {
        { false, "p4", "whatever", "p4" },
        { true, "sizeof(*", ")", "sizeof(*p2)" },
    };

    const std::string argExprFront = "&(";
    const std::string argExprBack = "->s)";

    SimpleString testHeader =
            "struct Struct1 { int a; };\n"
            "struct Struct2 { struct Struct1 s; };\n"
            "unsigned long function1(const signed int* p1, struct Struct2* p2, signed char* p3, short p4);\n";

    mock().installCopier( "std::string", stdStringCopier );

    for( auto testData : testDataList )
    {
        // Prepare
        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p2", override );
        expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

        expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
        expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
        expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
        expect::Config$::GetTypeOverride( config, "#short", nullptr );

        expect::Config$::OverrideSpec$::GetType( override, MockedType::MemoryBuffer );
        expect::Config$::OverrideSpec$::GetExprModFront( override, &argExprFront );
        expect::Config$::OverrideSpec$::GetExprModBack( override, &argExprBack );
        expect::Config$::OverrideSpec$::HasSizeExprPlaceholder( override, testData.hasPlaceholder );
        expect::Config$::OverrideSpec$::GetSizeExprFront( override, &testData.sizeExprFront );
        expect::Config$::OverrideSpec$::GetSizeExprBack( override, &testData.sizeExprBack );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "unsigned long function1(const int * p1, struct Struct2 * p2, signed char * p3, short p4)\n{\n"
            "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
                ".withMemoryBufferParameter(\"p2\", static_cast<const unsigned char *>(static_cast<const void *>(&(p2->s))), %s)"
                ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
            "}\n", testData.expectedCallExpr.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );
    }
}

/*
 * Check mock generation of a function with parameter override of type MemoryBuffer.
 */
TEST_EX( TypeOverrides, ParameterOverride_MemoryBuffer_FromArray )
{
    struct TestData
    {
        bool hasPlaceholder;
        std::string sizeExprFront;
        std::string sizeExprBack;
        std::string expectedCallExpr;
    };

    const std::vector<struct TestData> testDataList
    {
        { false, "p4", "whatever", "p4" },
        { true, "sizeof(", "[0])*p4", "sizeof(p2[0])*p4" },
    };

    const std::string argExprFront = "";
    const std::string argExprBack = "";

    SimpleString testHeader =
            "struct Struct1 { int a; };\n"
            "struct Struct2 { struct Struct1 s; };\n"
            "unsigned long function1(const signed int* p1, struct Struct2 p2[], signed char* p3, short p4);\n";

    mock().installCopier( "std::string", stdStringCopier );

    for( auto testData : testDataList )
    {
        // Prepare
        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
        expect::Config$::GetTypeOverride( config, "function1@", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p2", override );
        expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

        expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
        expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
        expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
        expect::Config$::GetTypeOverride( config, "#short", nullptr );

        expect::Config$::OverrideSpec$::GetType( override, MockedType::MemoryBuffer );
        expect::Config$::OverrideSpec$::GetExprModFront( override, &argExprFront );
        expect::Config$::OverrideSpec$::GetExprModBack( override, &argExprBack );
        expect::Config$::OverrideSpec$::HasSizeExprPlaceholder( override, testData.hasPlaceholder );
        expect::Config$::OverrideSpec$::GetSizeExprFront( override, &testData.sizeExprFront );
        expect::Config$::OverrideSpec$::GetSizeExprBack( override, &testData.sizeExprBack );

        // Exercise
        std::vector<std::string> results;
        unsigned int functionCount = ParseHeader( testHeader, *config, results );

        // Verify
        mock().checkExpectations();
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
            "unsigned long function1(const int * p1, struct Struct2 p2[], signed char * p3, short p4)\n{\n"
            "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1)"
                ".withMemoryBufferParameter(\"p2\", static_cast<const unsigned char *>(static_cast<const void *>(p2)), %s)"
                ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
            "}\n", testData.expectedCallExpr.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );
    }
}

/*
 * Check mock generation of a function with parameter override that skips a parameter.
 */
TEST_EX( TypeOverrides, ParameterOverride_Skip )
{
    // Prepare
    mock().installCopier( "std::string", stdStringCopier );

    Config* config = GetMockConfig();
    const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(1);
    expect::Config$::GetTypeOverride( config, "function1@", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p2", override );
    expect::Config$::GetTypeOverride( config, "function1#p3", nullptr );
    expect::Config$::GetTypeOverride( config, "function1#p4", nullptr );

    expect::Config$::GetTypeOverride( config, "@unsigned long", nullptr );
    expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
    expect::Config$::GetTypeOverride( config, "#signed char *", nullptr );
    expect::Config$::GetTypeOverride( config, "#short", nullptr );

    expect::Config$::OverrideSpec$::GetType( override, MockedType::Skip );

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
    CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with parameter override.
 */
TEST_EX( TypeOverrides, ReturnOverride )
{
    struct TestData
    {
        MockedType mockedType;
        std::string cpputestFunctionType;
        std::string argExprFront;
        std::string argExprBack;
    };

    const std::vector<struct TestData> testDataList
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

    SimpleString testHeader = "unsigned long function1(const signed int* p1, const char* p2);";

    for( auto testData : testDataList )
    {
        // Prepare
        Config* config = GetMockConfig();
        const Config::OverrideSpec* override = GetMockConfig_OverrideSpec(199);
        expect::Config$::GetTypeOverride( config, "function1@", override );
        expect::Config$::GetTypeOverride( config, "function1#p1", nullptr );
        expect::Config$::GetTypeOverride( config, "function1#p2", nullptr );

        expect::Config$::GetTypeOverride( config, "#const int *", nullptr );
        expect::Config$::GetTypeOverride( config, "#const char *", nullptr );

        expect::Config$::OverrideSpec$::GetType( override, testData.mockedType );
        expect::Config$::OverrideSpec$::GetExprModFront( override, &testData.argExprFront );
        expect::Config$::OverrideSpec$::GetExprModBack( override, &testData.argExprBack );

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
                "}\n", testData.argExprFront.c_str(), testData.cpputestFunctionType.c_str(), testData.argExprBack.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckMockCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
        mock().clear();
    }
}
