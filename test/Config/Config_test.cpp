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
 * Check that InterpretAsCpp is handled properly.
 */
TEST( Config, InterpretAsCpp )
{
    // Prepare
    Config testConfigTrue( true, "", false, std::vector<std::string>() );
    Config testConfigFalse( false, "", false, std::vector<std::string>() );

    // Exercise & Verify
    CHECK_TRUE( testConfigTrue.InterpretAsCpp() );
    CHECK_FALSE( testConfigFalse.InterpretAsCpp() );

    // Cleanup
}

/*
 * Check that LanguageStandard is handled properly.
 */
TEST( Config, LanguageStandard )
{
    // Prepare
    Config testConfigEmpty( false, "", false, std::vector<std::string>() );
    Config testConfigNonEmpty( false, "std-xxx", false, std::vector<std::string>() );

    // Exercise & Verify
    CHECK( testConfigEmpty.GetLanguageStandard().empty() );
    STRCMP_EQUAL( "std-xxx", testConfigNonEmpty.GetLanguageStandard().c_str() );

    // Cleanup
}

/*
 * Check that UseUnderlyingTypedefType is handled properly.
 */
TEST( Config, UseUnderlyingTypedefType )
{
    // Prepare
    Config testConfigTrue( false, "", true, std::vector<std::string>() );
    Config testConfigFalse( false, "", false, std::vector<std::string>() );

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
    Config testConfig( false, "", false, std::vector<std::string>() );

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
    Config testConfig( false, "", false, std::vector<std::string> { "function1#p=Int", "ns1::function2@=ConstPointer" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("ns1::function2@");

    // Verify
    const Config::OverrideSpec* testedOverride;

    testedOverride = override1;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );

    testedOverride = override2;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );

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
    Config testConfig( false, "", false, std::vector<std::string> { "#class1=Int", "@class2 *=ConstPointer" } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#class1");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@class2 *");

    // Verify
    const Config::OverrideSpec* testedOverride;

    testedOverride = override1;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );

    testedOverride = override2;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );

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
    Config testConfig( false, "", false, std::vector<std::string>
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
        "function1#p16=InputPOD",
        "function1#p17=MemoryBuffer:p17_size",
        "function1#p18=OutputPOD",
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
    CHECK( testConfig.GetTypeOverride("function1#p16") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p17") != NULL );
    CHECK( testConfig.GetTypeOverride("function1#p18") != NULL );

    // Cleanup
}

/*
 * Check that generic type override options allowed types are accepted.
 */
TEST( Config, GenericTypeOverrideOptions_AllowedTypes )
{
    // Prepare

    // Exercise
    Config testConfig( false, "", false, std::vector<std::string>
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
        "#type16=InputPOD",
        "#type17=MemoryBuffer:sizeof(*$)",
        "#type18=OutputPOD",
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
    CHECK( testConfig.GetTypeOverride("#type16") != NULL );
    CHECK( testConfig.GetTypeOverride("#type17") != NULL );
    CHECK( testConfig.GetTypeOverride("#type18") != NULL );

    // Cleanup
}

/*
 * Check that specific type override options with argument/size expression are handled properly.
 */
TEST( Config, SpecificTypeOverrideOptions_WithExpressions )
{
    // Prepare
    Config testConfig( false, "", false,
                       std::vector<std::string> { "function1#p=Int~($)",
                                                  "ns1::function1@=ConstPointer~&$",
                                                  "func2#p=InputOfType:TypeZ~$->getZ()",
                                                  "func3#p=OutputOfType:TypeW~(*$).getW()",
                                                  "func4#p=InputOfType:TypeM<OtherTypeA~$->getM()",
                                                  "func5#p=OutputOfType:TypeN<OtherTypeB~(*$).getN()",
                                                  "func6#p1=MemoryBuffer:p2",
                                                  "func7#p=MemoryBuffer:$",
                                                  "func8#p=MemoryBuffer:sizeof(*$)",
                                                  "func9#p1=MemoryBuffer:p3~&$",
                                                  "func10#p=MemoryBuffer:getSize($)~$.get()",
                                                } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("function1#p");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("ns1::function1@");
    const Config::OverrideSpec* override3 = testConfig.GetTypeOverride("func2#p");
    const Config::OverrideSpec* override4 = testConfig.GetTypeOverride("func3#p");
    const Config::OverrideSpec* override5 = testConfig.GetTypeOverride("func4#p");
    const Config::OverrideSpec* override6 = testConfig.GetTypeOverride("func5#p");
    const Config::OverrideSpec* override7 = testConfig.GetTypeOverride("func6#p1");
    const Config::OverrideSpec* override8 = testConfig.GetTypeOverride("func7#p");
    const Config::OverrideSpec* override9 = testConfig.GetTypeOverride("func8#p");
    const Config::OverrideSpec* override10 = testConfig.GetTypeOverride("func9#p1");
    const Config::OverrideSpec* override11 = testConfig.GetTypeOverride("func10#p");

    // Verify
    const Config::OverrideSpec* testedOverride;

    testedOverride = override1;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "(", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override2;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::ConstPointer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override3;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getZ()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeZ", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeZ", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override4;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "(*", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ").getW()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeW", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeW", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override5;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getM()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeM", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeA", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override6;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "(*", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ").getN()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeN", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeB", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override7;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_FALSE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "p2", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetSizeExprBack().c_str() );

    testedOverride = override8;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_TRUE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetSizeExprBack().c_str() );

    testedOverride = override9;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_TRUE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "sizeof(*", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( ")", testedOverride->GetSizeExprBack().c_str() );

    testedOverride = override10;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_FALSE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "p3", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetSizeExprBack().c_str() );

    testedOverride = override11;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".get()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_TRUE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "getSize(", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( ")", testedOverride->GetSizeExprBack().c_str() );

    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("ABC") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#q") );
    POINTERS_EQUAL( NULL, testConfig.GetTypeOverride("function1#p2") );

    // Cleanup
}

/*
 * Check that generic type override options with argument/size expression are handled properly.
 */
TEST( Config, GenericTypeOverrideOptions_WithExpressions )
{
    // Prepare
    Config testConfig( false, "", false,
                       std::vector<std::string> { "#const int *=Int~(*$)",
                                                  "@const int *=LongInt~&$",
                                                  "#typeX=InputOfType:TypeY~&($.getY())",
                                                  "#typeQ=OutputOfType:TypeQ~&$->getQ()",
                                                  "#typeI=InputOfType:TypeK<OtherTypeC~&($.getK())",
                                                  "#typeJ=OutputOfType:TypeL<OtherTypeD~&$->getL()", 
                                                  "#typeM=MemoryBuffer:p3~&$",
                                                  "#typeN=MemoryBuffer:getSize($)~$.get()",
                                                } );

    // Exercise
    const Config::OverrideSpec* override1 = testConfig.GetTypeOverride("#const int *");
    const Config::OverrideSpec* override2 = testConfig.GetTypeOverride("@const int *");
    const Config::OverrideSpec* override3 = testConfig.GetTypeOverride("#typeX");
    const Config::OverrideSpec* override4 = testConfig.GetTypeOverride("#typeQ");
    const Config::OverrideSpec* override5 = testConfig.GetTypeOverride("#typeI");
    const Config::OverrideSpec* override6 = testConfig.GetTypeOverride("#typeJ");
    const Config::OverrideSpec* override7 = testConfig.GetTypeOverride("#typeM");
    const Config::OverrideSpec* override8 = testConfig.GetTypeOverride("#typeN");

    // Verify
    const Config::OverrideSpec* testedOverride;

    testedOverride = override1;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::Int, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "(*", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ")", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override2;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::Long, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override3;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&(", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".getY())", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeY", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeY", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override4;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getQ()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeQ", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "TypeQ", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override5;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::InputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&(", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".getK())", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeK", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeC", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override6;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::OutputOfType, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "->getL()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "TypeL", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "OtherTypeD", testedOverride->GetExpectationArgTypeName().c_str() );

    testedOverride = override7;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "&", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_FALSE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "p3", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetSizeExprBack().c_str() );

    testedOverride = override8;
    CHECK( testedOverride != NULL );
    CHECK_EQUAL( (int)MockedType::MemoryBuffer, (int)testedOverride->GetType() );
    STRCMP_EQUAL( "", testedOverride->GetExprModFront().c_str() );
    STRCMP_EQUAL( ".get()", testedOverride->GetExprModBack().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExposedTypeName().c_str() );
    STRCMP_EQUAL( "", testedOverride->GetExpectationArgTypeName().c_str() );
    CHECK_TRUE( testedOverride->HasSizeExprPlaceholder() );
    STRCMP_EQUAL( "getSize(", testedOverride->GetSizeExprFront().c_str() );
    STRCMP_EQUAL( ")", testedOverride->GetSizeExprBack().c_str() );

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
                  Config( false, "", false, std::vector<std::string> { "function1#p" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1@" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#class1" } ) );

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1" } ) );


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
                  Config( false, "", false, std::vector<std::string> { "=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p#=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#function1#p=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type1#=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1@p=Int" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@type1@=Int" } ) );

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
                  Config( false, "", false, std::vector<std::string> { "function1#p=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=Int~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=Int~abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1@=Output" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1@=Skip" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=InputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=InputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=InputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1#p=InputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function2#p=OutputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function2#p=OutputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function2#p=OutputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function2#p=OutputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function3#p=MemoryBuffer:" } ) );

    // Cleanup
}

/*
 * Check that an exception is thrown when the type override options is not valid.
 */
TEST( Config, Exception_TypeOverride_GenericValueBadFormat )
{
    // Prepare

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1=" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type1=abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1=~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#class1=Int~" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type2=Int~abc" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1=Output" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1=Skip" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type3=InputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type3=InputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type3=InputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type3=InputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type4=OutputOfType:" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type4=OutputOfType:<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type4=OutputOfType:a<" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type4=OutputOfType:<a" } ) );

    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#type5=MemoryBuffer:" } ) );

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
                  Config( false, "", false, std::vector<std::string> { "function1#p=Int", "function1#p=Double" } ) );
}

/*
 * Check that an exception is thrown when an specific type override option function identifier is repeated.
 */
TEST( Config, Exception_FunctionReturnRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "function1@=Int", "function1@=Double" } ) );
}

/*
 * Check that an exception is thrown when an type override option type identifier is repeated.
 */
TEST( Config, Exception_TypeParameterRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "#class1=Int", "#class1=Double" } ) );
}

/*
 * Check that an exception is thrown when an type override option type identifier is repeated.
 */
TEST( Config, Exception_TypeReturnRepeated )
{
    // Prepare

    // Exercise & Verify
    CHECK_THROWS( std::runtime_error,
                  Config( false, "", false, std::vector<std::string> { "@class1=Int", "@class1=Double" } ) );
}
