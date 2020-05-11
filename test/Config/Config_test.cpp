/**
 * @file
 * @brief      Unit tests for the "Config" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifdef _MSC_VER
#pragma warning( disable: 26444 6011 )
#endif

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <string>
#include <vector>
#include <stdexcept>

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
    Config testConfigTrue( true, std::vector<std::string>() );
    Config testConfigFalse( false, std::vector<std::string>() );

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
    Config testConfig( false, std::vector<std::string>() );

    // Exercise & Verify
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("f1#p") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("f2@") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("#int*") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("@class1") );

    // Cleanup
}

/*
 * Check that specific type override options are handled properly with simple override.
 */
TEST( Config, SpecificTypeOverrideOptions_Simple )
{
    // Prepare
    Config testConfig( false, std::vector<std::string> { "function1#p=Int", "ns1::function2@=ConstPointer" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("ns1::function2@");

    // Verify
    CHECK( override1 != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override1->GetExprModBack().c_str() );

    CHECK( override2 != NULL );
    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)override2->GetType() );
    STRCMP_EQUAL( "", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1@") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#p2") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function2#p") );

    // Cleanup
}

/*
 * Check that generic type override options are handled properly with simple override.
 */
TEST( Config, GenericTypeOverrideOptions_Simple )
{
    // Prepare
    Config testConfig( false, std::vector<std::string> { "#class1=Int", "@class2 *=ConstPointer" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#class1");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@class2 *");

    // Verify
    CHECK( override1 != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override1->GetExprModBack().c_str() );

    CHECK( override2 != NULL );
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
 * Check that specific type override options allowed types are accepted.
 */
TEST( Config, SpecificTypeOverrideOptions_AllowedTypes )
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
        "function1#p12=InputOfType:MyType",
        "function1#p13=OutputOfType:MyType",
        "function1#p14=InputOfType:MyType<OtherType",
        "function1#p15=OutputOfType:MyType<OtherType",
    } );

    // Verify
    CHECK( testConfig.GetTypeOverride("function1#p1") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p2") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p3") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p4") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p5") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p6") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p7") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p8") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p9") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p10") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p11") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p12") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p13") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p14") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p15") != NULL );

    // Cleanup
}

/*
 * Check that generic type override options allowed types are accepted.
 */
TEST( Config, GenericTypeOverrideOptions_AllowedTypes )
{
    // Prepare

    // Exercise
    Config testConfig( false, std::vector<std::string>
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
        "#type12=InputOfType:MyType",
        "#type13=OutputOfType:MyType",
        "#type14=InputOfType:MyType<OtherType",
        "#type15=OutputOfType:MyType<OtherType",
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
    CHECK( testConfig.GetTypeOverride("#type12") != NULL );
    CHECK( testConfig.GetTypeOverride("#type13") != NULL );
    CHECK( testConfig.GetTypeOverride("#type14") != NULL );
    CHECK( testConfig.GetTypeOverride("#type15") != NULL );

    // Cleanup
}

/*
 * Check that specific type override options are handled properly with override with argument expression.
 */
TEST( Config, SpecificTypeOverrideOptions_ArgumentExpression )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string> { "function1#p=Int~($)",
                                                  "ns1::function1@=ConstPointer~&$",
                                                  "func2#p=InputOfType:TypeZ~$->getZ()",
                                                  "func3#p=OutputOfType:TypeW~(*$).getW()",
                                                  "func4#p=InputOfType:TypeM<OtherTypeA~$->getM()",
                                                  "func5#p=OutputOfType:TypeN<OtherTypeB~(*$).getN()" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("ns1::function1@");
    const Config::OverrideSpec* override3 = testConfig.GetTypeOverride("func2#p");
    const Config::OverrideSpec* override4 = testConfig.GetTypeOverride("func3#p");
    const Config::OverrideSpec* override5 = testConfig.GetTypeOverride("func4#p");
    const Config::OverrideSpec* override6 = testConfig.GetTypeOverride("func5#p");

    // Verify
    CHECK( override1 != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "(", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", override1->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", override1->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", override1->GetExpectationArgTypeName().c_str() );

    CHECK( override2 != NULL );
    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)override2->GetType() );
    STRCMP_EQUAL( "&", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", override2->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", override2->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", override2->GetExpectationArgTypeName().c_str() );

    CHECK( override3 != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)override3->GetType() );
    STRCMP_EQUAL( "", override3->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getZ()", override3->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeZ", override3->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeZ", override3->GetExpectationArgTypeName().c_str() );

    CHECK( override4 != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)override4->GetType() );
    STRCMP_EQUAL( "(*", override4->GetExprModFront().c_str() );
    STRCMP_EQUAL( ").getW()", override4->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeW", override4->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeW", override4->GetExpectationArgTypeName().c_str() );

    CHECK( override5 != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)override5->GetType() );
    STRCMP_EQUAL( "", override5->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getM()", override5->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeM", override5->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeA", override5->GetExpectationArgTypeName().c_str() );

    CHECK( override6 != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)override6->GetType() );
    STRCMP_EQUAL( "(*", override6->GetExprModFront().c_str() );
    STRCMP_EQUAL( ").getN()", override6->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeN", override6->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeB", override6->GetExpectationArgTypeName().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#p2") );

    // Cleanup
}

/*
 * Check that generic type override options are handled properly with override with argument expression.
 */
TEST( Config, GenericTypeOverrideOptions_ArgumentExpression )
{
    // Prepare
    Config testConfig( false,
                       std::vector<std::string> { "#const int *=Int~(*$)",
                                                  "@const int *=LongInt~&$",
                                                  "#typeX=InputOfType:TypeY~&($.getY())",
                                                  "#typeQ=OutputOfType:TypeQ~&$->getQ()",
                                                  "#typeI=InputOfType:TypeK<OtherTypeC~&($.getK())",
                                                  "#typeJ=OutputOfType:TypeL<OtherTypeD~&$->getL()" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#const int *");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@const int *");
    const Config::OverrideSpec* override3 = testConfig.GetTypeOverride("#typeX");
    const Config::OverrideSpec* override4 = testConfig.GetTypeOverride("#typeQ");
    const Config::OverrideSpec* override5 = testConfig.GetTypeOverride("#typeI");
    const Config::OverrideSpec* override6 = testConfig.GetTypeOverride("#typeJ");

    // Verify
    CHECK( override1 != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)override1->GetType() );
    STRCMP_EQUAL( "(*", override1->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", override1->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", override1->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", override1->GetExpectationArgTypeName().c_str() );

    CHECK( override2 != NULL );
    CHECK_EQUAL( (int)MockedType::Long, (int)override2->GetType() );
    STRCMP_EQUAL( "&", override2->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", override2->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", override2->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", override2->GetExpectationArgTypeName().c_str() );

    CHECK( override3 != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)override3->GetType() );
    STRCMP_EQUAL( "&(", override3->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".getY())", override3->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeY", override3->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeY", override3->GetExpectationArgTypeName().c_str() );

    CHECK( override4 != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)override4->GetType() );
    STRCMP_EQUAL( "&", override4->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getQ()", override4->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeQ", override4->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeQ", override4->GetExpectationArgTypeName().c_str() );

    CHECK( override5 != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)override5->GetType() );
    STRCMP_EQUAL( "&(", override5->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".getK())", override5->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeK", override5->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeC", override5->GetExpectationArgTypeName().c_str() );

    CHECK( override6 != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)override6->GetType() );
    STRCMP_EQUAL( "&", override6->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getL()", override6->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeL", override6->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeD", override6->GetExpectationArgTypeName().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("const int *") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("#const int &") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("@int") );

    // Cleanup
}

/*
 * Check that an exception is thrown when type override options is not valid.
 */
TEST( Config, Exception_TypeOverride_OverallBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#class1" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1" } ) );


    // Cleanup
}

/*
 * Check that an exception is thrown when keys of type override options is not valid.
 */
TEST( Config, Exception_TypeOverride_KeyBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p#=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#function1#p=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type1#=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@p=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@type1@=Int" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when values of type override options is not valid.
 */
TEST( Config, Exception_TypeOverride_SpecificValueBadFormat )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int~abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@=Output" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@=Skip" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=InputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=InputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=InputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=InputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function2#p=OutputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function2#p=OutputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function2#p=OutputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function2#p=OutputOfType:<a" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when the type override options is not valid.
 */
TEST( Config, Exception_TypeOverride_GenericValueBadFormat )
{
    // Prepare

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type1=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1=~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#class1=Int~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type2=Int~abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1=Output" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1=Skip" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type3=InputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type3=InputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type3=InputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type3=InputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type4=OutputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type4=OutputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type4=OutputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#type4=OutputOfType:<a" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when an specific type override option function identifier is repeated.
 */
TEST( Config, Exception_FunctionParameterRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1#p=Int", "function1#p=Double" } ) );
}

/*
 * Check that an exception is thrown when an specific type override option function identifier is repeated.
 */
TEST( Config, Exception_FunctionReturnRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "function1@=Int", "function1@=Double" } ) );
}

/*
 * Check that an exception is thrown when an type override option type identifier is repeated.
 */
TEST( Config, Exception_TypeParameterRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "#class1=Int", "#class1=Double" } ) );
}

/*
 * Check that an exception is thrown when an type override option type identifier is repeated.
 */
TEST( Config, Exception_TypeReturnRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, std::vector<std::string> { "@class1=Int", "@class1=Double" } ) );
}
