/**
 * @file
 * @brief      unit tests for the "Function" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "ClangParseHelper.hpp"
#include "ClangCompileHelper.hpp"
#include <vector>
#include <string>

#include "Function.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( Function )
{
    struct primitiveTypeData
    {
        std::string originalType;
        std::string mockedType;
        bool casted;
        std::string cpputestFunctionType;
    };

    const std::vector< struct primitiveTypeData > primitiveTypes {
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
        { "bool", "bool", false, "Bool" },
        { "float", "float", true, "Double" },
        { "double", "double", false, "Double" },
        { "wchar_t", "wchar_t", true, "Int" },
        { "char16_t", "char16_t", true, "UnsignedInt" },
        { "char32_t", "char32_t", true, "UnsignedInt" },
    };

    struct primitivePointedTypeData
    {
        std::string originalType;
        std::string mockedType;
    };

    const std::vector< struct primitivePointedTypeData > primitivePointedTypesWithoutString {
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
        { "bool", "bool" },
        { "float", "float" },
        { "double", "double" },
        { "wchar_t", "wchar_t" },
        { "char16_t", "char16_t" },
        { "char32_t", "char32_t" },
    };

    std::vector< struct primitivePointedTypeData > primitivePointedTypesWithString = primitivePointedTypesWithoutString;

    std::vector< struct primitivePointedTypeData > primitivePointedTypesWithStringAndVoid = primitivePointedTypesWithoutString;

    std::vector< struct primitivePointedTypeData > primitivePointedTypesWithoutStringWithVoid = primitivePointedTypesWithoutString;

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

/*
 * Check that a function with definition is not mocked.
 */
TEST( Function, WithDefinition )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "void function1() {}";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 0, results.size() );

    // Cleanup
}

/*
 * Check that a function declared twice is mocked just once.
 */
TEST( Function, DoubleDeclaration )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "void function1();\n"
            "void function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 2, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1()\n{\n"
                  "    mock().actualCall(\"function1\");\n"
                  "}\n", results[0].c_str() );

    // Cleanup
}

//*************************************************************************************************
//                                DIFFERENT RETURNS / NO PARAMETERS
//*************************************************************************************************

/*
 * Check mock generation of a function without parameters nor return value.
 */
TEST( Function, VoidReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "void function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1()\n{\n"
                  "    mock().actualCall(\"function1\");\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning primitive type value.
 */
TEST( Function, PrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat( "%s function1();", typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult;
        if( typeData.casted )
        {
            expectedResult= StringFromFormat(
                    "%s function1()\n{\n"
                    "    return static_cast<%s>( mock().actualCall(\"function1\").return%sValue() );\n"
                    "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        }
        else
        {
            expectedResult= StringFromFormat(
                    "%s function1()\n{\n"
                    "    return mock().actualCall(\"function1\").return%sValue();\n"
                    "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        }
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning enum value.
 */
TEST( Function, EnumReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum Enum1 { A, B, C };\n"
            "Enum1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Enum1 function1()\n{\n"
                  "    return static_cast<Enum1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning scoped enum value.
 */
TEST( Function, ScopedEnumReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum class Enum1 { A, B, C };\n"
            "Enum1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Enum1 function1()\n{\n"
                  "    return static_cast<Enum1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a class.
 */
TEST( Function, ClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1 function1()\n{\n"
                  "    return * static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a template class.
 */
TEST( Function, TemplateClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int> function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1<int> function1()\n{\n"
                  "    return * static_cast<const Class1<int> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for primitive type.
 */
TEST( Function, PrimitiveTypeTypedefReturnNoParameters )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult= StringFromFormat(
                "Type1 function1()\n{\n"
                "    return static_cast<Type1>( mock().actualCall(\"function1\").return%sValue() );\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for an enum.
 */
TEST( Function, EnumTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef enum { X, Y, Z } Type1;\n"
            "Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a scoped enum.
 */
TEST( Function, ScopedEnumTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z };\n"
            "typedef Enum1 Type1;\n"
            "Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnIntValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a class.
 */
TEST( Function, ClassTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;\n"
            "Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return * static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a template class.
 */
TEST( Function, TemplateClassTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "typedef Class1<long> Type1;\n"
            "Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return * static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning pointer to non-const void value.
 */
TEST( Function, PointerToVoidReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "void* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnPointerValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning pointer to const void value.
 */
TEST( Function, PointerToConstVoidReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "const void* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const void * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnConstPointerValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning pointer to non-const primitive type value.
 */
TEST( Function, PointerToPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat( "%s* function1();", typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult= StringFromFormat(
                "%s * function1()\n{\n"
                "    return static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning pointer to const primitive type value.
 */
TEST( Function, PointerToConstPrimitiveTypeReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutString )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat( "const %s* function1();", typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult= StringFromFormat(
                "const %s * function1()\n{\n"
                "    return static_cast<const %s *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                "}\n", typeData.mockedType.c_str(), typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a string.
 */
TEST( Function, StringReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "const char* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const char * function1()\n{\n"
                  "    return mock().actualCall(\"function1\").returnStringValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const class.
 */
TEST( Function, PointerToClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "Class1* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1 * function1()\n{\n"
                  "    return static_cast<Class1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const class.
 */
TEST( Function, PointerToConstClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "const Class1* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Class1 * function1()\n{\n"
                  "    return static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a non-const template class.
 */
TEST( Function, PointerToTemplateClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "Class1<int>* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Class1<int> * function1()\n{\n"
                  "    return static_cast<Class1<int> *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a pointer to a const template class.
 */
TEST( Function, PointerToConstTemplateClassReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "const Class1<char>* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Class1<char> * function1()\n{\n"
                  "    return static_cast<const Class1<char> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning left/right-value reference to non-const primitive type value.
 */
TEST( Function, ReferenceToPrimitiveTypeReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat( "%s%s function1();", typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            SimpleString expectedResult= StringFromFormat(
                    "%s %s function1()\n{\n"
                    "    return * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                    "}\n", typeData.mockedType.c_str(), referenceType.c_str(), typeData.mockedType.c_str() );
            STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function without parameters and returning left/right-value reference to const primitive type value.
 */
TEST( Function, ReferenceToConstPrimitiveTypeReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat( "const %s%s function1();", typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            SimpleString expectedResult= StringFromFormat(
                    "const %s %s function1()\n{\n"
                    "    return * static_cast<const %s *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                    "}\n", typeData.mockedType.c_str(), referenceType.c_str(), typeData.mockedType.c_str() );
            STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function without parameters and returning a left/right-value reference to a non-const class.
 */
TEST( Function, ReferenceToClassReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "class Class1 { int member1[100]; };\n"
                "Class1%s function1();",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Class1 %s function1()\n{\n"
                "    return * static_cast<Class1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a left/right-value reference to a const class.
 */
TEST( Function, ReferenceToConstClassReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "class Class1 { int member1[100]; };\n"
                "const Class1%s function1();",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "const Class1 %s function1()\n{\n"
                "    return * static_cast<const Class1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a left/right-value reference to a non-const template class.
 */
TEST( Function, ReferenceToTemplateClassReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "template<class T1> class Class1 { T1 member1[100]; };\n"
                "Class1<int>%s function1();",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "Class1<int> %s function1()\n{\n"
                "    return * static_cast<Class1<int> *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a left/right-value reference to a const template class.
 */
TEST( Function, ReferenceToConstTemplateClassReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "template<class T1> class Class1 { T1 member1[100]; };\n"
                "const Class1<int>%s function1();",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "const Class1<int> %s function1()\n{\n"
                "    return * static_cast<const Class1<int> *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning pointer to typedef for const void value.
 */
TEST( Function, ConstVoidTypedefPointerReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const void Type1;\n"
            "Type1* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 * function1()\n{\n"
                  "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning pointer to const typedef for void value.
 */
TEST( Function, VoidTypedefConstPointerReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef void Type1;\n"
            "const Type1* function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type1 * function1()\n{\n"
                  "    return static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning pointer to typedef for non-const primitive type value.
 */
TEST( Function, PrimitiveTypeTypedefPointerReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "Type1* function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 * function1()\n{\n"
                      "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning pointer to typedef for const primitive type value.
 */
TEST( Function, ConstPrimitiveTypeTypedefPointerReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s Type1;\n"
                "Type1* function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 * function1()\n{\n"
                      "    return static_cast<Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning pointer to const typedef for primitive type value.
 */
TEST( Function, PrimitiveTypeTypedefConstPointerReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "const Type1* function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type1 * function1()\n{\n"
                      "    return static_cast<const Type1 *>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to non-const primitive type.
 */
TEST( Function, PointerToPrimitiveTypeTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 function1()\n{\n"
                      "    return static_cast<Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a pointer to const primitive type.
 */
TEST( Function, PointerToConstPrimitiveTypeTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "Type1 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type1 function1()\n{\n"
                      "    return static_cast<Type1>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a const typedef which is a pointer to non-const primitive type.
 */
TEST( Function, PointerToPrimitiveTypeTypedefConstReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "const Type1 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type1 function1()\n{\n"
                      "    return static_cast<const Type1>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a string.
 */
TEST( Function, StringTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type1 function1()\n{\n"
                  "    return static_cast<Type1>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a const typedef for a string.
 */
TEST( Function, StringTypedefConstReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "const Type1 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type1 function1()\n{\n"
                  "    return static_cast<const Type1>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a typedef for a reference to non-const primitive type.
 */
TEST( Function, ReferenceToPrimitiveTypeTypedefReturnNoParameters )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat(
                    "typedef %s%s Type1;\n"
                    "Type1 function1();",
                    typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            SimpleString expectedResult= StringFromFormat(
                    "Type1 function1()\n{\n"
                    "    return static_cast<Type1>( * static_cast<%s *>( mock().actualCall(\"function1\").returnPointerValue() ) );\n"
                    "}\n", typeData.mockedType.c_str() );
            STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a primitive type.
 */
TEST( Function, PrimitiveTypePointerTypedefTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a primitive type.
 */
TEST( Function, PrimitiveTypePointerTypedefConstTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a primitive type.
 */
TEST( Function, PrimitiveTypePointerTypedefTypedefConstReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "const Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type2 function1()\n{\n"
                      "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST( Function, ConstPrimitiveTypePointerTypedefTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a pointer to a const primitive type.
 */
TEST( Function, ConstPrimitiveTypePointerTypedefConstTypedefReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef const Type1 Type2;\n"
                "Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "Type2 function1()\n{\n"
                      "    return static_cast<Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a pointer to a const primitive type.
 */
TEST( Function, ConstPrimitiveTypePointerTypedefTypedefConstReturnNoParameters )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "typedef Type1 Type2;\n"
                "const Type2 function1();",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "const Type2 function1()\n{\n"
                      "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnConstPointerValue() );\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function without parameters and returning a double non-const typedef for a string.
 */
TEST( Function, StringTypedefTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "Type2 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type2 function1()\n{\n"
                  "    return static_cast<Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double non-const / const typedef for a string.
 */
TEST( Function, StringTypedefConstTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef const Type1 Type2;\n"
            "Type2 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "Type2 function1()\n{\n"
                  "    return static_cast<Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function without parameters and returning a double const / non-const typedef for a string.
 */
TEST( Function, StringConstTypedefTypedefReturnNoParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "const Type2 function1();";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "const Type2 function1()\n{\n"
                  "    return static_cast<const Type2>( mock().actualCall(\"function1\").returnStringValue() );\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

//*************************************************************************************************
//                                 VOID RETURN / SINGLE PARAMETER
//*************************************************************************************************

/*
 * Check mock generation of a function with primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPrimitiveTypeParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat( "void function1(%s p);", typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(%s p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str(), typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with enum parameter and without return value.
 */
TEST( Function, VoidReturnEnumParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum Enum1 { X, Y, Z };\n"
            "void function1(Enum1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Enum1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with scoped enum parameter and without return value.
 */
TEST( Function, VoidReturnScopedEnumParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z };\n"
            "void function1(Enum1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Enum1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with class parameter and without return value.
 */
TEST( Function, VoidReturnClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with template class parameter and without return value.
 */
TEST( Function, VoidReturnTemplateClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short> p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1<short> p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPrimitiveTypeTypedefParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult= StringFromFormat(
                "void function1(Type1 p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a typedef for an enum parameter and without return value.
 */
TEST( Function, VoidReturnEnumTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef enum { X, Y, Z, K } Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a scoped enum parameter and without return value.
 */
TEST( Function, VoidReturnScopedEnumTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "enum class Enum1 { X, Y, Z, W };\n"
            "typedef Enum1 Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withIntParameter(\"p\", static_cast<int>(p));\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef of a class parameter and without return value.
 */
TEST( Function, VoidReturnClassTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef of a template class parameter and without return value.
 */
TEST( Function, VoidReturnTemplateClassTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template <class T1> class Class1 { T1 member1[100]; };\n"
            "typedef Class1<long> Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", &p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to non-const void parameter and without return value.
 */
TEST( Function, VoidReturnPointerToVoidParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "void function1(void* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(void * p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPointerToPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "void function1(%s* p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(%s * p)\n{\n"
                "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with pointer to const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPointerToConstPrimitiveTypeParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "void function1(const %s* p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(const %s * p)\n{\n"
                "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                "}\n", typeData.mockedType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with string parameter and without return value.
 */
TEST( Function, VoidReturnStringParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "void function1(const char* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const char * p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to non-const class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(Class1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to const class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToConstClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "void function1(const Class1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Class1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to non-const template class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToTemplateClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(Class1<short>* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Class1<short> * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1<short>\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to const template class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToConstTemplateClassParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "template<class T1> class Class1 { T1 member1[100]; };\n"
            "void function1(const Class1<short>* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Class1<short> * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with left/right-value reference to non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToPrimitiveTypeParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat(
                    "void function1(%s%s p);",
                    typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            SimpleString expectedResult = StringFromFormat(
                      "void function1(%s %s p)\n{\n"
                      "    mock().actualCall(\"function1\").withOutputParameter(\"p\", &p);\n"
                      "}\n", typeData.mockedType.c_str(), referenceType.c_str() );
            STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function with left/right-value reference to const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToConstPrimitiveTypeParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithoutString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat(
                    "void function1(const %s%s p);",
                    typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            SimpleString expectedResult = StringFromFormat(
                      "void function1(const %s %s p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", &p);\n"
                      "}\n", typeData.mockedType.c_str(), referenceType.c_str() );
            STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function with left/right-value reference to non-const class parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToClassParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "class Class1 { int member1[100]; };\n"
                "void function1(Class1%s p);",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(Class1 %s p)\n{\n"
                "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1\", \"p\", &p);\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with left/right-value reference to const class parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToConstClassParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "class Class1 { int member1[100]; };\n"
                "void function1(const Class1%s p);",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(const Class1 %s p)\n{\n"
                "    mock().actualCall(\"function1\").withParameterOfType(\"Class1\", \"p\", &p);\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with left/right-value reference to non-const template class parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToTemplateClassParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "template<class T1> class Class1 { T1 member1[100]; };\n"
                "void function1(Class1<short>%s p);",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(Class1<short> %s p)\n{\n"
                "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Class1<short>\", \"p\", &p);\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with left/right-value reference to const template class parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToConstTemplateClassParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "template<class T1> class Class1 { T1 member1[100]; };\n"
                "void function1(const Class1<short>%s p);",
                referenceType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult = StringFromFormat(
                "void function1(const Class1<short> %s p)\n{\n"
                "    mock().actualCall(\"function1\").withParameterOfType(\"Class1<short>\", \"p\", &p);\n"
                "}\n", referenceType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with pointer to typedf for void type parameter and without return value.
 */
TEST( Function, VoidReturnVoidTypedefPointerParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef void Type1;\n"
            "void function1(Type1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with pointer to typedef for non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPrimitiveTypeTypedefPointerParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(Type1* p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withOutputParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with pointer to typedef for const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnConstPrimitiveTypeTypedefPointerParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s Type1;\n"
                "void function1(Type1* p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with pointer to const typedef for non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPrimitiveTypeTypedefConstPointerParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "void function1(const Type1* p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type1 * p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a pointer to typedef of a class parameter and without return value.
 */
TEST( Function, VoidReturnClassTypedefPointerParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "void function1(Type1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withOutputParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a pointer to const typedef of a class parameter and without return value.
 */
TEST( Function, VoidReturnConstClassTypedefPointerParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1 Type1;"
            "void function1(Type1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const pointer to typedef of a class parameter and without return value.
 */
TEST( Function, VoidReturnClassTypedefConstPointerParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1 Type1;"
            "void function1(const Type1* p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type1 * p)\n{\n"
                  "    mock().actualCall(\"function1\").withParameterOfType(\"Type1\", \"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with typedef for a pointer to non-const void parameter and without return value.
 */
TEST( Function, VoidReturnPointerToVoidTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef void* Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a pointer to non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPointerToPrimitiveTypeTypedefParameter )
{
    for( auto typeData : primitivePointedTypesWithString )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a typedef for a pointer to const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPointerToConstPrimitiveTypeTypedefParameter )
{
    for( auto typeData : primitivePointedTypesWithoutStringWithVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef const %s* Type1;\n"
                "void function1(Type1 p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a const typedef which is a pointer to non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPointerToPrimitiveTypeTypedefConstParameter )
{
    for( auto typeData : primitivePointedTypesWithStringAndVoid )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s* Type1;\n"
                "void function1(const Type1 p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        STRCMP_EQUAL( "void function1(const Type1 p)\n{\n"
                      "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                      "}\n", results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a typedef for a string parameter and without return value.
 */
TEST( Function, VoidReturnStringTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a const typedef for a string parameter and without return value.
 */
TEST( Function, VoidReturnStringTypedefConstParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "void function1(const Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef of a pointer to class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToClassTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef Class1* Type1;"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef of a pointer to const class parameter and without return value.
 */
TEST( Function, VoidReturnPointerToConstClassTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "class Class1 { int member1[100]; };\n"
            "typedef const Class1* Type1;"
            "void function1(Type1 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                  "    mock().actualCall(\"function1\").withConstPointerParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a typedef for a reference to non-const primitive type parameter and without return value.
 */
TEST( Function, VoidReturnReferenceToPrimitiveTypeTypedefParameter )
{
    const std::vector<std::string> referenceTypes = { "&", "&&" };

    for( auto referenceType : referenceTypes )
    {
        for( auto typeData : primitivePointedTypesWithString )
        {
            // Prepare
            Config config( false );
            SimpleString testHeader = StringFromFormat(
                    "typedef %s%s Type1;\n"
                    "void function1(Type1 p);",
                    typeData.originalType.c_str(), referenceType.c_str() );
            std::vector<std::string> results;
            unsigned int functionCount = 0;

            // Exercise
            ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
            {
                functionCount++;

                Function function( cursor, config );

                if( function.IsMockable() )
                {
                    results.push_back( function.GenerateMock() );
                }
            } );

            // Verify
            CHECK_EQUAL( 1, functionCount );
            CHECK_EQUAL( 1, results.size() );
            STRCMP_EQUAL( "void function1(Type1 p)\n{\n"
                          "    mock().actualCall(\"function1\").withPointerParameter(\"p\", &p);\n"
                          "}\n", results[0].c_str() );
            CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

            // Cleanup
        }
    }
}

/*
 * Check mock generation of a function with a double typedef for primitive type parameter and without return value.
 */
TEST( Function, VoidReturnPrimitiveTypeDoubleTypedefParameter )
{
    for( auto typeData : primitiveTypes )
    {
        // Prepare
        Config config( false );
        SimpleString testHeader = StringFromFormat(
                "typedef %s Type1;\n"
                "typedef Type1 Type2;\n"
                "void function1(Type2 p);",
                typeData.originalType.c_str() );
        std::vector<std::string> results;
        unsigned int functionCount = 0;

        // Exercise
        ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
        {
            functionCount++;

            Function function( cursor, config );

            if( function.IsMockable() )
            {
                results.push_back( function.GenerateMock() );
            }
        } );

        // Verify
        CHECK_EQUAL( 1, functionCount );
        CHECK_EQUAL( 1, results.size() );
        SimpleString expectedResult= StringFromFormat(
                "void function1(Type2 p)\n{\n"
                "    mock().actualCall(\"function1\").with%sParameter(\"p\", p);\n"
                "}\n", typeData.cpputestFunctionType.c_str() );
        STRCMP_EQUAL( expectedResult.asCharString(), results[0].c_str() );
        CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

        // Cleanup
    }
}

/*
 * Check mock generation of a function with a double non-const typedef for a string parameter and without return value.
 */
TEST( Function, VoidReturnStringTypedefTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "void function1(Type2 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double non-const / const typedef for a string parameter and without return value.
 */
TEST( Function, VoidReturnStringTypedefConstTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef const Type1 Type2;\n"
            "void function1(Type2 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with a double const / non-const typedef for a string parameter and without return value.
 */
TEST( Function, VoidReturnStringConstTypedefTypedefParameter )
{
    // Prepare
    Config config( false );
    SimpleString testHeader =
            "typedef const char* Type1;\n"
            "typedef Type1 Type2;\n"
            "void function1(const Type2 p);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "void function1(const Type2 p)\n{\n"
                  "    mock().actualCall(\"function1\").withStringParameter(\"p\", p);\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}

/*
 * Check mock generation of a function with return and multiple parameters.
 */
TEST( Function, ReturnAndMultipleParameters )
{
    // Prepare
    Config config( false );
    SimpleString testHeader = "unsigned long function1(const signed int* p1, const char* p2, signed char* p3, short p4);";
    std::vector<std::string> results;
    unsigned int functionCount = 0;

    // Exercise
    ClangParseHelper::ParseFunctions( testHeader.asCharString(), [&]( CXCursor cursor )
    {
        functionCount++;

        Function function( cursor, config );

        if( function.IsMockable() )
        {
            results.push_back( function.GenerateMock() );
        }
    } );

    // Verify
    CHECK_EQUAL( 1, functionCount );
    CHECK_EQUAL( 1, results.size() );
    STRCMP_EQUAL( "unsigned long function1(const int * p1, const char * p2, signed char * p3, short p4)\n{\n"
                  "    return mock().actualCall(\"function1\").withConstPointerParameter(\"p1\", p1).withStringParameter(\"p2\", p2)"
                       ".withOutputParameter(\"p3\", p3).withIntParameter(\"p4\", p4).returnUnsignedLongIntValue();\n"
                  "}\n", results[0].c_str() );
    CHECK_TRUE( ClangCompileHelper::CheckCompilation( testHeader.asCharString(), results[0] ) );

    // Cleanup
}
