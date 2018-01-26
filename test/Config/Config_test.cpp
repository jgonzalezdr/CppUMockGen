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
 * Check that the UseUnderlyingTypedefType is handled properly.
 */
TEST( Config, UseUnderlyingTypedefType )
{
    // Prepare
    Config testConfigTrue( true, std::vector<std::string>() );
    Config testConfigFalse( false, std::vector<std::string>() );

    // Exercise & Verify
    CHECK_TRUE( testConfigTrue.UseUnderlyingTypedefType() );
    CHECK_FALSE( testConfigFalse.UseUnderlyingTypedefType() );

    // Cleanup
}

/*
 * Check that the override options are handled properly when empty.
 */
TEST( Config, OverrideOptions_Empty )
{
    // Prepare
    Config testConfig( false, std::vector<std::string>() );

    // Exercise & Verify
    POINTERS_EQUAL( NULL, testConfig.GetOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("ABC") );

    // Cleanup
}

/*
 * Check that the override options are handled properly with simple override.
 */
TEST( Config, OverrideOptions_Simple )
{
    // Prepare
    Config testConfig( false, std::vector<std::string> { "function1#p=Int", "ns1::function2#p1=ConstPointer" } );
    std::string pname1 = "pname1";
    std::string pname2 = "pname2";

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetOverride("ns1::function2#p1");
    override1->UpdateArgExpr( pname1 );
    override2->UpdateArgExpr( pname2 );

    // Verify
    STRCMP_EQUAL( "Int", override1->GetType().c_str() );
    STRCMP_EQUAL( "ConstPointer", override2->GetType().c_str() );
    STRCMP_EQUAL( "pname1", pname1.c_str() );
    STRCMP_EQUAL( "pname2", pname2.c_str() );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#p2") );

    // Cleanup
}

/*
 * Check that the override options are handled properly with override with argument expression.
 */
TEST( Config, OverrideOptions_ArgumentExpression )
{
    // Prepare
    Config testConfig( false, std::vector<std::string> { "function1#p=Int/($)", "ns1::function2#p1=ConstPointer/&$" } );
    std::string pname1 = "pname1";
    std::string pname2 = "pname2";

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetOverride("ns1::function2#p1");
    override1->UpdateArgExpr( pname1 );
    override2->UpdateArgExpr( pname2 );

    // Verify
    STRCMP_EQUAL( "Int", override1->GetType().c_str() );
    STRCMP_EQUAL( "ConstPointer", override2->GetType().c_str() );
    STRCMP_EQUAL( "(pname1)", pname1.c_str() );
    STRCMP_EQUAL( "&pname2", pname2.c_str() );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetOverride("function1#p2") );

    // Cleanup
}

/*
 * Check that an exception is thrown when the override options is not valid.
 */
TEST( Config, Exception_OverrideBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=/" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int/" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int/abc" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when an override option function identifier is repeated.
 */
TEST( Config, Exception_FunctionRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int", "function1#p=Double" } ) );
}
