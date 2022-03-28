/**
 * @file
 * @brief      Unit tests for the "App" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

/*===========================================================================
 *                              INCLUDES
 *===========================================================================*/

#include <map>
#include <functional>

#include "Options_expect.hpp"

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "App.hpp"

#include "Parser_expect.hpp"
#include "ConsoleColorizer_expect.hpp"
#include "Config_expect.hpp"
#include "OutputFileParser_expect.hpp"

using CppUMockGen::IgnoreParameter;

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

class StdVectorOfStringsComparator : public MockNamedValueComparator
{
public:
    bool isEqual(const void* object1, const void* object2)
    {
        const std::vector<std::string> *o1 = (const std::vector<std::string>*) object1;
        const std::vector<std::string> *o2 = (const std::vector<std::string>*) object2;

        return (*o1) == (*o2);
    }

    SimpleString valueToString(const void* object)
    {
        SimpleString ret;
        const std::vector<std::string> *o = (const std::vector<std::string>*) object;
        for( unsigned int i = 0; i < o->size(); i++ )
        {
            ret += StringFromFormat("<%u>%s\n", i, (*o)[i].c_str() );
        }
        return ret;
    }
};

StdVectorOfStringsComparator stdVectorOfStringsComparator;

class StdOstreamCopier : public MockNamedValueCopier
{
public:
    virtual void copy(void* out, const void* in)
    {
        *(std::ostream*)out << *(const std::string*)in;
    }
};

StdOstreamCopier stdOstreamCopier;

static const std::filesystem::path tempDirPath = std::filesystem::temp_directory_path();
static const std::filesystem::path outDirPath = ( tempDirPath / "" );
static const std::string inputFilename = "foo.h";
static const std::string mockOutputFilename = "foo_mock.cpp";
static const std::string mockOutputFilePath = (outDirPath / mockOutputFilename).generic_string();
static const std::string expectationHeaderOutputFilename = "foo_expect.hpp";
static const std::string expectationHeaderOutputFilePath = (outDirPath / expectationHeaderOutputFilename).generic_string();
static const std::string expectationImplOutputFilename = "foo_expect.cpp";
static const std::string expectationImplOutputFilePath = (outDirPath / expectationImplOutputFilename).generic_string();

static void CheckFileContains( const std::string &filepath, const std::string &contents )
{
    std::ifstream file( filepath );
    if( !file.is_open() )
    {
        FAIL( "File cannot be opened" );
    }
    std::stringstream strStream;
    strStream << file.rdbuf();

    STRCMP_EQUAL( contents.c_str(), strStream.str().c_str() );
}

static const std::string EMPTY_STRING;

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( App )
{
    std::filesystem::path initialDir;
    std::string outputFilepath1;
    std::string outputFilepath2;
    std::string outputFilepath3;

    TEST_SETUP()
    {
        initialDir = std::filesystem::current_path();
    }

    TEST_TEARDOWN()
    {
        std::filesystem::current_path( initialDir );

        if( !outputFilepath1.empty() )
        {
            std::filesystem::remove( outputFilepath1 );
        }
        if( !outputFilepath2.empty() )
        {
            std::filesystem::remove( outputFilepath2 );
        }
        if( !outputFilepath3.empty() )
        {
            std::filesystem::remove( outputFilepath3 );
        }
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that help option displays usage
 */
TEST( App, Help )
{
    // Prepare
    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 33;
    const char* argv[] = { "don't", "care" };

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, true );
    expect::Options$::GetHelpText( IgnoreParameter::YES, "###HELP_TEXT###" );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "###HELP_TEXT###", output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that version option displays the version
 */
TEST( App, Version )
{
    // Prepare
    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 22;
    const char* argv[] = { "don't", "care" };

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, true );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "CppUMockGenFoo vF.O.O", output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if no input is specified, an error is displayed
 */
TEST( App, NoInput )
{
    // Prepare
    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 0;
    const char* argv[] = { "don't", "care" };

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, "" );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( "No input file specified", error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that if no output is specified, an error is displayed
 */
TEST( App, NoOutput )
{
    // Prepare
    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 99;
    const char* argv[] = { "don't", "care" };

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified",
                     error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that mock generation is requested properly and saved to an output directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_OutDir_WithEndingPathSeparator )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    outputFilepath1 = mockOutputFilePath;
    std::filesystem::remove( outputFilepath1 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 1;
    const char* argv[] = { "don't", "care" };

    std::string outDirPathStr = outDirPath.generic_string();

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT1#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outDirPathStr.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to an output directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_OutDir_WithoutEndingPathSeparator )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    outputFilepath1 = mockOutputFilePath;
    std::filesystem::remove( outputFilepath1 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outDirPathStr = outDirPath.generic_string();
    outDirPathStr.pop_back();
    
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT1#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outDirPathStr.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ( "Mock generated into '" + outputFilepath1 + "'" ).c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to the current directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_NoOutFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    const char* inputFilename = "bar";
    const std::string outputFilename = "bar_mock.cpp";
    outputFilepath1 = (outDirPath / outputFilename).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT2#####";
    std::string userCode = "~~USER CODE 234~~~";

    std::filesystem::current_path( tempDirPath );

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilename.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), "", &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilename + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to a named output file
 */
TEST( App, MockOutput_OutFile_CppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_mock.cpp" ).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT3#####";
    std::string userCode = "~~USER CODE 23443~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to a named output file
 */
TEST( App, MockOutput_OutFile_CcExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_mock.cc" ).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT3#####";
    std::string userCode = "~~USER CODE 3244~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to a named output file
 */
TEST( App, MockOutput_OutFile_OtherExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my_mock.hpp").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.hpp.cpp" ).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT334#####";
    std::string userCode = "~~USER CODE 643~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and saved to a named output file
 */
TEST( App, MockOutput_OutFile_NoExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my_mock").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp" ).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT34455#####";
    std::string userCode = "~~USER CODE 1~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    //CHECK_EQUAL( 0, 1 );
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText );
}

/*
 * Check that mock generation is requested properly and printed to console
 */
TEST( App, MockOutput_ConsoleOutput )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT4#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if the output file cannot be opened, an error is displayed
 */
TEST( App, MockOutput_CannotOpenFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::filesystem::path nonExistantDirPath = (outDirPath / "NonExistantDirectory123898876354874" / "");
    std::string nonExistantDirpathStr = nonExistantDirPath.generic_string();
    std::string outputFilepathStr = (nonExistantDirPath / mockOutputFilename).generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, nonExistantDirpathStr.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );

    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepathStr.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   int ret = app.Execute( argc, argv );

   // Verify
   CHECK_EQUAL( 1, ret );
   STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( ("Mock output file '" + outputFilepathStr + "' could not be opened").c_str(), error.str().c_str() );
   STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that parsing in C++ mode is requested properly when a C++ file is parsed
 */
TEST( App, MockOutput_InterpretAsCpp_CppHeader )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string inputFilenameCpp = "foo.hpp";

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilenameCpp.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilenameCpp.c_str(), IgnoreParameter::YES, true, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that parsing in C++ mode is requested properly when forced from the command line
 */
TEST( App, MockOutput_InterpretAsCpp_Forced )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, true );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( true, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that the language standard is requested properly
 */
TEST( App, MockOutput_LanguageStandard )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "lang-std" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "lang-std", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that using underlying typedef types is requested properly
 */
TEST( App, MockOutput_UseUnderlyingTypedefType )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, true );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", true, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that include paths are passed properly to the parser
 */
TEST( App, MockOutput_IncludePaths )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that include parameter override options are passed properly to the configuration
 */
TEST( App, MockOutput_TypeOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions = { "foo#bar=String", "foo@=Int/&$", "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that generation options are passed for expectations generation.
 */
TEST( App, MockOutput_GenerationOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "-t ####SOME_OPTIONS#### -f whatever" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-t ####SOME_OPTIONS#### -f whatever", "", "", &outputText );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if the input file cannot be parsed successfully, an error is displayed
 */
TEST( App, MockOutput_ParseError )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, false );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 2, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Output could not be generated due to errors parsing the input file '" + inputFilename + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that expectation functions generation is requested properly and saved to an output directory (output filename deduced from input filename)
 */
TEST( App, ExpectationOutput_OutDir )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    outputFilepath1 = expectationHeaderOutputFilePath;
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = expectationImplOutputFilePath;
    std::filesystem::remove( outputFilepath2 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outDirPathStr = outDirPath.generic_string();

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT1#####";
    std::string outputText2 = "#####TEXT2#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outDirPathStr.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and saved to the current directory (output filename deduced from input filename)
 */
TEST( App, ExpectationOutput_NoOutFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    const char* inputFilename = "bar";
    const std::string outputFilename1 = "bar_expect.hpp";
    const std::string outputFilename2 = "bar_expect.cpp";

    outputFilepath1 = (outDirPath / outputFilename1).generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / outputFilename2).generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT22#####";
    std::string outputText2 = "#####TEXT33#####";

    std::filesystem::current_path( tempDirPath );

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilename1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilename1 + "' and '" + outputFilename2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationOutput_OutFile_HppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationOutput_OutFile_CppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepath2.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationOutput_OutFile_OtherExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my_expect.foo").generic_string();

    outputFilepath1 = (outDirPath / "my_expect.foo.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.foo.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationOutput_OutFile_NoExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
}

/*
 * Check that expectation functions generation is requested properly and printed to console
 */
TEST( App, ExpectationOutput_ConsoleOutput )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT4455#####";
    std::string outputText2 = "#####TEXT5642#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", "@", &outputText2 );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2).c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if the output file cannot be opened, an error is displayed
 */
TEST( App, ExpectationOutput_CannotOpenFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputDir = (outDirPath / "NonExistantDirectory123898876354874" / "").generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputDir.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectation header output file '" + outputDir + expectationHeaderOutputFilename + "' could not be opened").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that generation options are passed for expectations generation.
 */
TEST( App, ExpectationOutput_GenerationOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT4455#####";
    std::string outputText2 = "#####TEXT5642#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "-t ####SOME_OPTIONS#### -f whatever" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "-t ####SOME_OPTIONS#### -f whatever", "", &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "-t ####SOME_OPTIONS#### -f whatever", "@", &outputText2 );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2).c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if the input file cannot be parsed successfully, an error is displayed
 */
TEST( App, ExpectationOutput_ParseError )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, false );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, false );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 2, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Output could not be generated due to errors parsing the input file '" + inputFilename + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Both )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathMock = (outDirPath / "my_mock").generic_string();
    std::string outputFilepathExpect = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT0943#####";
    std::string outputText2 = "#####TEXT2083#####";
    std::string outputText3 = "#####TEXT7818#####";
    std::string userCode = "~~USER CODE 66561~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathMock.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathExpect.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to an output directory 
 * (output filename deduced from input filename)
 */
TEST( App, CombinedMockAndExpectationOutput_OutDir )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    outputFilepath1 = mockOutputFilePath;
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = expectationHeaderOutputFilePath;
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = expectationImplOutputFilePath;
    std::filesystem::remove( outputFilepath3 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outDirPathStr = outDirPath.generic_string();

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT1#####";
    std::string outputText2 = "#####TEXT2#####";
    std::string outputText3 = "#####TEXT3#####";
    std::string userCode = "~~USER CODE 481455~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outDirPathStr.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to the current directory
 * (output filename deduced from input filename)
 */
TEST( App, CombinedMockAndExpectationOutput_NoOutFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    const char* inputFilename = "bar";
    const std::string outputFilename1 = "bar_mock.cpp";
    const std::string outputFilename2 = "bar_expect.hpp";
    const std::string outputFilename3 = "bar_expect.cpp";

    outputFilepath1 = (outDirPath / outputFilename1).generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / outputFilename2).generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / outputFilename3).generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT22#####";
    std::string outputText2 = "#####TEXT33#####";
    std::string outputText3 = "#####TEXT44#####";
    std::string userCode = "~~USER CODE 36777~~~";

    std::filesystem::current_path( tempDirPath );

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilename1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), "", &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilename2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilename1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilename2 + "' and '" + outputFilename3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Mock_CppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_mock_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_mock_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT343#####";
    std::string outputText2 = "#####TEXT283#####";
    std::string outputText3 = "#####TEXT863#####";
    std::string userCode = "~~USER CODE 22677~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Expect_HppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_expect_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT843#####";
    std::string outputText2 = "#####TEXT683#####";
    std::string outputText3 = "#####TEXT463#####";
    std::string userCode = "~~USER CODE 56434~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepath2.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Expect_CppExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    outputFilepath1 = (outDirPath / "my_expect_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT443#####";
    std::string outputText2 = "#####TEXT383#####";
    std::string outputText3 = "#####TEXT193#####";
    std::string userCode = "~~USER CODE 267878~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepath3.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Mock_OtherExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my.foo").generic_string();

    outputFilepath1 = (outDirPath / "my.foo.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my.foo_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my.foo_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT743#####";
    std::string outputText2 = "#####TEXT783#####";
    std::string outputText3 = "#####TEXT713#####";
    std::string userCode = "~~USER CODE 224~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Mock_NoExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my").generic_string();

    outputFilepath1 = (outDirPath / "my.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT043#####";
    std::string outputText2 = "#####TEXT283#####";
    std::string outputText3 = "#####TEXT718#####";
    std::string userCode = "~~USER CODE 345787~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Expect_OtherExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my.foo").generic_string();

    outputFilepath1 = (outDirPath / "my.foo_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my.foo.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my.foo.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT743A#####";
    std::string outputText2 = "#####TEXT783M#####";
    std::string outputText3 = "#####TEXT713C#####";
    std::string userCode = "~~USER CODE 9543~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that combined mock and expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, CombinedMockAndExpectationOutput_OutFile_Expect_NoExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathOther = (outDirPath / "my").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT043#####";
    std::string outputText2 = "#####TEXT283#####";
    std::string outputText3 = "#####TEXT718#####";
    std::string userCode = "~~USER CODE 767678~~~";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathOther.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &userCode );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", userCode.c_str(), expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that expectation functions generation is requested properly and printed to console
 */
TEST( App, CombinedMockAndExpectationOutput_Mock_ConsoleOutput )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT44553#####";
    std::string outputText2 = "#####TEXT56424#####";
    std::string outputText3 = "#####TEXT12345#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", "@", &outputText3 );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2 + outputText3).c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that expectation functions generation is requested properly and printed to console
 */
TEST( App, CombinedMockAndExpectationOutput_Expect_ConsoleOutput )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT453#####";
    std::string outputText2 = "#####TEXT524#####";
    std::string outputText3 = "#####TEXT125#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", "", &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", "@", &outputText3 );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2 + outputText3).c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that base directory is requested properly
 */
TEST( App, MockOutput_BaseDirectory )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string baseDirectory = outDirPath.parent_path().generic_string();

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####FOO1#####";
    std::string outputText2 = "#####FOO2#####";
    std::string outputText3 = "#####FOO3#####";

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, "@" );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, "" );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, baseDirectory.c_str() );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, false );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", baseDirectory.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", baseDirectory.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", "@", &outputText3 );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2 + outputText3).c_str(), output.str().c_str() );
    STRCMP_EQUAL( "", error.str().c_str() );
}

/*
 * Check that if the specified base directory does not exist, an error is displayed.
 */
TEST( App, MockOutput_BaseDirectory_NotExists )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string baseDirectory = ( outDirPath / "NonExistantDirectory123898876354874" ).generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, false );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, baseDirectory.c_str() );

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Base directory path '" + baseDirectory + "' is not an existing directory").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
}

/*
 * Check that regeneration is handled properly.
 */
TEST( App, Regeneration_FromMock )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathMock = (outDirPath / "my_mock").generic_string();
    std::string outputFilepathExpect = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####FOO1#####";
    std::string outputText2 = "#####FOO2#####";
    std::string outputText3 = "#####FOO3#####";
    std::string generationOptions = "~~~GENERATION OPTIONS 12344===";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathMock.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathExpect.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, true );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::OutputFileParser$::OutputFileParser$ctor();
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &generationOptions );
    expect::Options$::Parse( IgnoreParameter::YES, generationOptions.c_str() );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "",  expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "",  expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that regeneration is handled properly.
 */
TEST( App, Regeneration_FromExpectationHeader )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathMock = (outDirPath / "my_mock").generic_string();
    std::string outputFilepathExpect = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####FOO1#####";
    std::string outputText2 = "#####FOO2#####";
    std::string outputText3 = "#####FOO3#####";
    std::string generationOptions = "~~~GENERATION OPTIONS 12344===";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathMock.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathExpect.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, true );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::OutputFileParser$::OutputFileParser$ctor(2);
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath2.c_str() );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &generationOptions );
    expect::Options$::Parse( IgnoreParameter::YES, generationOptions.c_str() );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "",  expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "",  expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}

/*
 * Check that regeneration is handled properly.
 */
TEST( App, Regeneration_FromExpectationImpl )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    int argc = 2;
    const char* argv[] = { "don't", "care" };

    std::string outputFilepathMock = (outDirPath / "my_mock").generic_string();
    std::string outputFilepathExpect = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_mock.cpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    outputFilepath3 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath3 );

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####FOO1#####";
    std::string outputText2 = "#####FOO2#####";
    std::string outputText3 = "#####FOO3#####";
    std::string generationOptions = "~~~GENERATION OPTIONS 12344===";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Options$::Options$ctor();
    expect::Options$::Parse( IgnoreParameter::YES, argc, argv );
    expect::Options$::IsHelpRequested( IgnoreParameter::YES, false );
    expect::Options$::IsVersionRequested( IgnoreParameter::YES, false );
    expect::Options$::GetInputPath( IgnoreParameter::YES, inputFilename.c_str() );
    expect::Options$::IsMockRequested( IgnoreParameter::YES, true );
    expect::Options$::IsExpectationsRequested( IgnoreParameter::YES, true );
    expect::Options$::GetMockOutputPath( IgnoreParameter::YES, outputFilepathMock.c_str() );
    expect::Options$::GetExpectationsOutputPath( IgnoreParameter::YES, outputFilepathExpect.c_str() );
    expect::Options$::GetBaseDirectory( IgnoreParameter::YES, "" );
    expect::Options$::IsRegenerationRequested( IgnoreParameter::YES, true );
    expect::Options$::InterpretAsCpp( IgnoreParameter::YES, false );
    expect::Options$::GetLanguageStandard( IgnoreParameter::YES, "" );
    expect::Options$::UseUnderlyingTypedef( IgnoreParameter::YES, false );
    expect::Options$::GetTypeOverrides( IgnoreParameter::YES, typeOverrideOptions );
    expect::Options$::GetIncludePaths( IgnoreParameter::YES, includePaths );
    expect::Options$::GetGenerationOptions( IgnoreParameter::YES, "" );

    expect::OutputFileParser$::OutputFileParser$ctor(3);
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath1.c_str() );
    expect::OutputFileParser$::GetUserCode( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath2.c_str() );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &EMPTY_STRING );
    expect::OutputFileParser$::Parse( IgnoreParameter::YES, outputFilepath3.c_str() );
    expect::OutputFileParser$::GetGenerationOptions( IgnoreParameter::YES, &generationOptions );
    expect::Options$::Parse( IgnoreParameter::YES, generationOptions.c_str() );

    expect::Config$::Config$ctor( false, "", false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "",  expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "",  expectedBaseDirPath.c_str(), &outputText2 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath2.c_str(), &outputText3 );
    expect::ConsoleColorizer$::SetColor( 4, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( argc, argv );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath2 + "' and '" + outputFilepath3 + "'").c_str(), error.str().c_str() );
    STRCMP_EQUAL( "", output.str().c_str() );
    CheckFileContains( outputFilepath1, outputText1 );
    CheckFileContains( outputFilepath2, outputText2 );
    CheckFileContains( outputFilepath3, outputText3 );
}
