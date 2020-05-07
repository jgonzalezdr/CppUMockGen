/**
 * @file
 * @brief      Unit tests for the "App" class
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

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <direct.h>

#include "App.hpp"
#include "FileHelper.hpp"

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

static const std::string tempDirPath = std::string(std::getenv("TEMP"));
static const std::string outDirPath = tempDirPath;
static const std::string inputFilename = "foo.h";
static const std::string mockOutputFilename = "foo_mock.cpp";
static const std::string mockOutputFilePath = outDirPath + PATH_SEPARATOR + mockOutputFilename;
static const std::string expectationHeaderOutputFilename = "foo_expect.hpp";
static const std::string expectationHeaderOutputFilePath = outDirPath + PATH_SEPARATOR + expectationHeaderOutputFilename;
static const std::string expectationImplOutputFilename = "foo_expect.cpp";
static const std::string expectationImplOutputFilePath = outDirPath + PATH_SEPARATOR + expectationImplOutputFilename;

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( App )
{
    std::string initialDir;
    std::string outputFilepath1;
    std::string outputFilepath2;
    std::string outputFilepath3;

    TEST_SETUP()
    {
        initialDir = getcwd( NULL, 0 );
    }

    TEST_TEARDOWN()
    {
        chdir( initialDir.c_str() );
        if( !outputFilepath1.empty() )
        {
            std::remove( outputFilepath1.c_str() );
        }
        if( !outputFilepath2.empty() )
        {
            std::remove( outputFilepath2.c_str() );
        }
        if( !outputFilepath3.empty() )
        {
            std::remove( outputFilepath3.c_str() );
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
    CHECK_EQUAL( 0, error.tellp() )

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
    CHECK_EQUAL( 0, error.tellp() )

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
    CHECK_EQUAL( 0, output.tellp() )

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
    CHECK_EQUAL( 0, output.tellp() )

    // Cleanup
}

/*
 * Check that mock generation is requested properly and saved to an output directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_OutDir )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    outputFilepath1 = mockOutputFilePath;
    std::remove( outputFilepath1.c_str() );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outDirPath.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT1#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", &outputText );
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
 * Check that mock generation is requested properly and saved to the current directory (output filename deduced from input filename)
 */
TEST( App, MockOutput_CurrentDir )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    const char* inputFilename = "bar";
    const std::string outputFilename = "bar_mock.cpp";
    outputFilepath1 = outDirPath + PATH_SEPARATOR + outputFilename;
    std::remove( outputFilepath1.c_str() );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename, "-m" };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT2#####";

    chdir( tempDirPath.c_str() );

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", &outputText );
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

    outputFilepath1 = outDirPath + PATH_SEPARATOR + "mymock.cpp";
    std::remove( outputFilepath1.c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT3#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", &outputText );
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

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####TEXT4#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", &outputText );

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

    std::string outputDir = outDirPath + PATH_SEPARATOR + "NonExistantDirectory123898876354874" + PATH_SEPARATOR;

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", outputDir.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

   // Exercise
   int ret = app.Execute( (int) args.size(), args.data() );

   // Verify
   CHECK_EQUAL( 1, ret );
   STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
   STRCMP_CONTAINS( ("Mock output file '" + outputDir + mockOutputFilename + "' could not be opened").c_str(), error.str().c_str() );
   CHECK_EQUAL( 0, output.tellp() )

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

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-x" };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, true, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-x ", &outputText );

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

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( true, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-u ", &outputText );

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

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "", &outputText );

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
TEST( App, MockOutput_paramOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-p", "foo#bar=String", "-p", "foo@=Int/&$" };

    std::vector<std::string> paramOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-p foo#bar=String -p foo@=Int/&$ ", &outputText );

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
TEST( App, MockOutput_typeOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-t", "#foo=String", "-t", "@const bar=Int/&$" };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions = { "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-t #foo=String -t \"@const bar=Int/&$\" ", &outputText );

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

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, false );
    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 2, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Output could not be generated due to errors parsing the input file '" + inputFilename + "'").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() )

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
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = expectationImplOutputFilePath;
    std::remove( outputFilepath2.c_str() );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outDirPath.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT1#####";
    std::string outputText2 = "#####TEXT2#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    outputFilepath1 = outDirPath + PATH_SEPARATOR + outputFilename1;
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = outDirPath + PATH_SEPARATOR + outputFilename2;
    std::remove( outputFilepath2.c_str() );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename, "-e" };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT22#####";
    std::string outputText2 = "#####TEXT33#####";

    chdir( tempDirPath.c_str() );

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename, IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    outputFilepath1 = outDirPath + PATH_SEPARATOR + "my_expect.hpp";
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = outDirPath + PATH_SEPARATOR + "my_expect.cpp";
    std::remove( outputFilepath2.c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    outputFilepath1 = outDirPath + PATH_SEPARATOR + "my_expect.hpp";
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = outDirPath + PATH_SEPARATOR + "my_expect.cpp";
    std::remove( outputFilepath2.c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepath2.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    std::string outputFilepathOther = outDirPath + PATH_SEPARATOR + "my_expect.blablabla";

    outputFilepath1 = outDirPath + PATH_SEPARATOR + "my_expect.hpp";
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = outDirPath + PATH_SEPARATOR + "my_expect.cpp";
    std::remove( outputFilepath2.c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepathOther.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    std::string outputFilepathOther = outDirPath + PATH_SEPARATOR + "my_expect";

    outputFilepath1 = outDirPath + PATH_SEPARATOR + "my_expect.hpp";
    std::remove( outputFilepath1.c_str() );

    outputFilepath2 = outDirPath + PATH_SEPARATOR + "my_expect.cpp";
    std::remove( outputFilepath2.c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputFilepathOther.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT43#####";
    std::string outputText2 = "#####TEXT83#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText1 = "#####TEXT4455#####";
    std::string outputText2 = "#####TEXT5642#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, includePaths, &error, true );
    expect::Parser$::GenerateExpectationHeader( IgnoreParameter::YES, "", &outputText1 );
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

    std::string outputDir = outDirPath + PATH_SEPARATOR + "NonExistantDirectory123898876354874" + PATH_SEPARATOR;

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-e", outputDir.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    //printf("Hello\n");
    int ret = app.Execute( (int) args.size(), args.data() );
    //printf("Goodbye\n");

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Expectation header output file '" + outputDir + expectationHeaderOutputFilename + "' could not be opened").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() )

    // Cleanup
}
