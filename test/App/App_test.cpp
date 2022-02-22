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

    bool CheckFileContains( const std::string &filepath, const std::string &contents )
    {
        std::ifstream file( filepath );
        if( !file.is_open() )
        {
            return false;
        }
        std::stringstream strStream;
        strStream << file.rdbuf();

        return strStream.str() == contents;
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-h" };

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "FooBar\nUsage:\n  CppUMockGenFoo", output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-v" };

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "CppUMockGenFoo vF.O.O", output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-x" };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( "No input file specified", error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str() };

     expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified",
                     error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
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

    std::string outDirPathStr = outDirPath.generic_string();
    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outDirPathStr.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT1#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText ) );

    // Cleanup
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

    std::string outDirPathStr = outDirPath.generic_string();
    outDirPathStr.pop_back();
    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outDirPathStr.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT1#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ( "Mock generated into '" + outputFilepath1 + "'" ).c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText ) );

    // Cleanup
}

/*
 * Check that mock generation is requested properly and saved to the current directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_CurrentDir )
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename, "-m" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT2#####";

    std::filesystem::current_path( tempDirPath );

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilename + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText ) );

    // Cleanup
}

/*
 * Check that mock generation is requested properly and saved to a named output file
 */
TEST( App, MockOutput_OutFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = (outDirPath / "mymock.cpp" ).generic_string();
    std::filesystem::remove( outputFilepath1 );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outputFilepath1.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT3#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Mock generated into '" + outputFilepath1 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText ) );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT4#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::string outputDir = (outDirPath / "NonExistantDirectory123898876354874" / "").generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outputDir.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   int ret = app.Execute( (int) args.size(), args.data() );

   // Verify
   CHECK_EQUAL( 1, ret );
   STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( ("Mock output file '" + outputDir + mockOutputFilename + "' could not be opened").c_str(), error.str().c_str() );
   CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
}

/*
 * Check that parsing in C++ mode is requested properly
 */
TEST( App, MockOutput_InterpretAsCpp )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "--cpp" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, true, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-x", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "--std", "lang-std" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "lang-std", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-s lang-std", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-u" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( true, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-u", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-I", "IncludePath1", "-I", "IncludePath2" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
}

/*
 * Check that include parameter override options are passed properly to the configuration
 */
TEST( App, MockOutput_specificTypeOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-t", "foo#bar=String", "-t", "foo@=Int/&$" };

    std::vector<std::string> typeOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-t foo#bar=String -t foo@=Int/&$", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
}

/*
 * Check that type parameter override options are passed properly to the configuration
 */
TEST( App, MockOutput_genericTypeOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-t", "#foo=String", "-t", "@const bar=Int/&$" };

    std::vector<std::string> typeOverrideOptions = { "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-t #foo=String -t \"@const bar=Int/&$\"", "", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
}

/*
 * Check that mock generation is requested properly and printed to console
 */
TEST( App, MockOutput_ParseError )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, false );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 2, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Output could not be generated due to errors parsing the input file '" + inputFilename + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
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

    std::string outDirPathStr = outDirPath.generic_string();
    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outDirPathStr.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT1#####";
    std::string outputText2 = "#####TEXT2#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
}

/*
 * Check that expectation functions generation is requested properly and saved to the current directory (output filename deduced from input filename)
 */
TEST( App, ExpectationOutput_CurrentDir )
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename, "-e" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT22#####";
    std::string outputText2 = "#####TEXT33#####";

    std::filesystem::current_path( tempDirPath );

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilename1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilename1 + "' and '" + outputFilename2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationMockOutput_OutFile_Header )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepath1.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationMockOutput_OutFile_Impl )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepath2.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationMockOutput_OutFile_OtherExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::string outputFilepathOther = (outDirPath / "my_expect.blablabla").generic_string();

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepathOther.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
}

/*
 * Check that expectation functions generation is requested properly and saved to a named output file
 */
TEST( App, ExpectationMockOutput_OutFile_OtherNoExtension )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::string outputFilepathOther = (outDirPath / "my_expect").generic_string();

    outputFilepath1 = (outDirPath / "my_expect.hpp").generic_string();
    std::filesystem::remove( outputFilepath1 );

    outputFilepath2 = (outDirPath / "my_expect.cpp").generic_string();
    std::filesystem::remove( outputFilepath2 );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepathOther.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    std::string expectedBaseDirPath = outDirPath.parent_path().generic_string();

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", expectedBaseDirPath.c_str(), &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", outputFilepath1.c_str(), &outputText2 );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_CONTAINS( "SUCCESS:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectations generated into '" + outputFilepath1 + "' and '" + outputFilepath2 + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );
    CHECK( CheckFileContains( outputFilepath1, outputText1 ) );
    CHECK( CheckFileContains( outputFilepath2, outputText2 ) );

    // Cleanup
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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", "@" };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT4455#####";
    std::string outputText2 = "#####TEXT5642#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", "", &outputText1 );
    expect::Parser$::GenerateExpectationImpl( IgnoreParameter::YES, "", "@", &outputText2 );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( (outputText1 + outputText2).c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::string outputDir = (outDirPath / "NonExistantDirectory123898876354874" / "").generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputDir.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectation header output file '" + outputDir + expectationHeaderOutputFilename + "' could not be opened").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
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

    std::string baseDirectory = outDirPath.parent_path().generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-B", baseDirectory.c_str() };

    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", baseDirectory.c_str(), &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
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

    std::string baseDirectory = ( outDirPath / "NonExistantDirectory123898876354874" ).generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-B", baseDirectory.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Base directory path '" + baseDirectory + "' is not an existing directory").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
}
