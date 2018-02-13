/**
 * @file
 * @brief      Unit tests for the "Config" class
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

#include "Config.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( Config )
{
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that UseUnderlyingTypedefType is handled properly.
 */
TEST( Config, UseUnderlyingTypedefType )
{
    // Prepare
    Config testConfigTrue( true, std::vector<std::string>(), std::vector<std::string>() );
    Config testConfigFalse( false, std::vector<std::string>(), std::vector<std::string>() );

    // Exercise & Verify
    CHECK_TRUE( testConfigTrue.UseUnderlyingTypedefType() );
    CHECK_FALSE( testConfigFalse.UseUnderlyingTypedefType() );

    // Cleanup
}

/*
 * Check that override options are handled properly when empty.
 */
TEST( Config, OverrideOptions_Empty )
{
    // Prepare
    Config testConfig( false, std::vector<std::string>(), std::vector<std::string>() );

    // Exercise & Verify
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("f1#p") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("f2@") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("#int*") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("@class1") );

    // Cleanup
}

/*
 * Check that parameter override options are handled properly with simple override.
 */
TEST( Config, ParameterOverrideOptions_Simple )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string> { "function1#p=Int", "ns1::function2@=ConstPointer" },
                       std::vector<std::string>() );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetParameterOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetParameterOverride("ns1::function2@");

    // Verify
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override1->GetExprModBack().c_str() );

    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)override2->GetType() );
    STRCMP_EQUAL( "", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1@") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#p2") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function2#p") );

    // Cleanup
}

/*
 * Check that type override options are handled properly with simple override.
 */
TEST( Config, TypeOverrideOptions_Simple )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string>(),
                       std::vector<std::string> { "#class1=Int", "@class2 *=ConstPointer" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#class1");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@class2 *");

    // Verify
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override1->GetExprModBack().c_str() );

    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)override2->GetType() );
    STRCMP_EQUAL( "", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("class1") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("#class1 *") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("class2") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("@class2") );

    // Cleanup
}

/*
 * Check that parameter override options allowed types are accepted.
 */
TEST( Config, ParameterOverrideOptions_AllowedTypes )
{
    // Prepare

    // Exercise
    Config testConfig( false, std::vector<std::string>
    {
        "function1#p1=Int",
        "function1#p2=UnsignedInt",
        "function1#p3=LongInt",
        "function1#p4=UnsignedLongInt",
        "function1#p5=Bool",
        "function1#p6=String",
        "function1#p7=Pointer",
        "function1#p8=ConstPointer",
        "function1#p9=Output",
        "function1#p10=Double",
        "function1#p11=Skip",
    }, std::vector<std::string>() );

    // Verify
    CHECK( testConfig.GetParameterOverride("function1#p1") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p2") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p3") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p4") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p5") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p6") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p7") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p8") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p9") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p10") != NULL );
    CHECK( testConfig.GetParameterOverride("function1#p11") != NULL );

    // Cleanup
}

/*
 * Check that type override options allowed types are accepted.
 */
TEST( Config, TypeOverrideOptions_AllowedTypes )
{
    // Prepare

    // Exercise
    Config testConfig( false, std::vector<std::string>(), std::vector<std::string>
    {
        "#type1=Int",
        "#type2=UnsignedInt",
        "#type3=LongInt",
        "#type4=UnsignedLongInt",
        "#type5=Bool",
        "#type6=String",
        "#type7=Pointer",
        "#type8=ConstPointer",
        "#type9=Output",
        "#type10=Double",
        "#type11=Skip",
    } );

    // Verify
    CHECK( testConfig.GetTypeOverride("#type1") != NULL );
    CHECK( testConfig.GetTypeOverride("#type2") != NULL );
    CHECK( testConfig.GetTypeOverride("#type3") != NULL );
    CHECK( testConfig.GetTypeOverride("#type4") != NULL );
    CHECK( testConfig.GetTypeOverride("#type5") != NULL );
    CHECK( testConfig.GetTypeOverride("#type6") != NULL );
    CHECK( testConfig.GetTypeOverride("#type7") != NULL );
    CHECK( testConfig.GetTypeOverride("#type8") != NULL );
    CHECK( testConfig.GetTypeOverride("#type9") != NULL );
    CHECK( testConfig.GetTypeOverride("#type10") != NULL );
    CHECK( testConfig.GetTypeOverride("#type11") != NULL );

    // Cleanup
}

/*
 * Check that parameter override options are handled properly with override with argument expression.
 */
TEST( Config, ParameterOverrideOptions_ArgumentExpression )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string> { "function1#p=Int/($)", "ns1::function1@=ConstPointer/&$" },
                       std::vector<std::string>() );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetParameterOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetParameterOverride("ns1::function1@");

    // Verify
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "(", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", override1->GetExprModBack().c_str() );

    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)override2->GetType() );
    STRCMP_EQUAL( "&", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetParameterOverride("function1#p2") );

    // Cleanup
}

/*
 * Check that type override options are handled properly with override with argument expression.
 */
TEST( Config, TypeOverrideOptions_ArgumentExpression )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string>(),
                       std::vector<std::string> { "#const int *=Int/(*$)", "@const int *=LongInt/&$" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#const int *");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@const int *");

    // Verify
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "(*", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", override1->GetExprModBack().c_str() );

    CHECK_EQUAL( (int)MockedType::Long, (int)override2->GetType() );
    STRCMP_EQUAL( "&", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("const int *") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("#const int &") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("@int") );

    // Cleanup
}

/*
 * Check that an exception is thrown when the parameter override options is not valid.
 */
TEST( Config, Exception_ParameterOverrideBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "=Int" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=abc" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=/" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int/" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int/abc" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@=Output" }, std::vector<std::string>() ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@=Skip" }, std::vector<std::string>() ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when the type override options is not valid.
 */
TEST( Config, Exception_TypeOverrideBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "#class1" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "@class1=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "#type1=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "@class1=/" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "#class1=Int/" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "#type2=Int/abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "@class1=Output" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "@class1=Skip" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when an parameter override option function identifier is repeated.
 */
TEST( Config, Exception_FunctionRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int", "function1#p=Double" }, std::vector<std::string>() ) );
}

/*
 * Check that an exception is thrown when an type override option type identifier is repeated.
 */
TEST( Config, Exception_TypeRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string>(), std::vector<std::string> { "@class1=Int", "@class1=Double" } ) );
}
