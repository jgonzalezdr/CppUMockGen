/**
 * @file
 * @brief      Unit tests for the "ConfigFile" class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018 Jesus Gonzalez. All rights reserved.
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
static const std::filesystem::path outDirPath = tempDirPath;
static const std::string inputFilename = "foo.h";

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( ConfigFile )
{
    std::filesystem::path initialDir;
    std::string outputFilepath1;
    std::string outputFilepath2;
    std::string outputFilepath3;
    std::ofstream tempFile;

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

    bool CheckFileContains( const std::filesystem::path &filepath, const std::string &contents )
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

    void SetupTempFile( const std::filesystem::path &filepath, const SimpleString &contents )
    {
        tempFile.open( filepath );
        tempFile << contents.asCharString();
        tempFile.close();
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that include paths are passed properly to the parser
 */
TEST( ConfigFile, IncludePaths )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = (tempDirPath / "ConfigFile.cfg").generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 -I IncludePath2" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-f", outputFilepath1.c_str(), "-m", "@" };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
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
TEST( ConfigFile, ParamOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-p foo#bar=String\n-p foo@=Int/&$" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-f", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
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
TEST( ConfigFile, TypeOverrideOptions )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-t\n#foo=String\n-t\n\"@const bar=Int/&$\"" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-f", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions = { "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
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
 * Check that if the config file cannot be opened, an error is displayed
 */
TEST( ConfigFile, CannotOpenFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::string nonExistingFile = ( tempDirPath / "NonExistantDirectory123898876354874" / "ConfigFile.cfg" ).generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "-f", nonExistingFile.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ("Configuration file '" + nonExistingFile + "' could not be opened").c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
}

/*
 * Check that other config files specified in a config file are parsed properly
 */
TEST( ConfigFile, IncludeOtherConfigFile_1Level )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 -I IncludePath2\n-f NestedDir/NestedConfigFile.cfg" );

    std::filesystem::path nestedDirPath = tempDirPath / "NestedDir";
    std::filesystem::create_directory( nestedDirPath );

    outputFilepath2 = ( nestedDirPath / "NestedConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath2, "-p foo#bar=String\n-p foo@=Int/&$" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-f", outputFilepath1.c_str(), "-m", "@" };

    std::vector<std::string> paramOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-p foo#bar=String -p foo@=Int/&$ ", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
    std::filesystem::remove_all( nestedDirPath );
    outputFilepath2.clear();
}

/*
 * Check that other config files specified in a config file are parsed properly
 */
TEST( ConfigFile, IncludeOtherConfigFile_2Level )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 -I IncludePath2\n-f NestedDir/NestedConfigFile.cfg" );

    std::filesystem::path nestedDirPath = tempDirPath / "NestedDir";
    std::filesystem::create_directory( nestedDirPath );

    outputFilepath2 = ( nestedDirPath / "NestedConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath2, "-p foo#bar=String\n-p foo@=Int/&$\n-f ../ConfigFile2.cfg" );

    outputFilepath3 = ( tempDirPath / "ConfigFile2.cfg" ).generic_string();
    SetupTempFile( outputFilepath3, "-t\n#foo=String\n-t\n\"@const bar=Int/&$\"" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-f", outputFilepath1.c_str(), "-m", "@" };

    std::vector<std::string> paramOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> typeOverrideOptions = { "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-p foo#bar=String -p foo@=Int/&$ -t #foo=String -t \"@const bar=Int/&$\" ", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
    std::filesystem::remove_all( nestedDirPath );
    outputFilepath2.clear();
}

/*
 * Check that if an included config file cannot be opened, an error is displayed
 */
TEST( ConfigFile, IncludeOtherConfigFile_CannotOpenFile )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    std::string nonExistingFile = ( tempDirPath / "NonExistantDirectory123898876354874" / "ConfigFile.cfg" ).generic_string();

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, ("-I IncludePath1 -I IncludePath2\n-f " + nonExistingFile).c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-f", outputFilepath1.c_str(), "-m", "@" };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ( "Configuration file '" + nonExistingFile + "' could not be opened" ).c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
}

/*
 * Check that if a config file is included recursively, it is ignored
 */
TEST( ConfigFile, IncludeOtherConfigFile_Recursive )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 -I IncludePath2\n-f ConfigFile.cfg\n-p foo#bar=String\n-p foo@=Int/&$" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-f", outputFilepath1.c_str(), "-m", "@" };

    std::vector<std::string> paramOverrideOptions = { "foo#bar=String", "foo@=Int/&$" };
    std::vector<std::string> typeOverrideOptions;
    std::vector<std::string> includePaths = { "IncludePath1", "IncludePath2" };
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
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
 * Check that extra white spaces are parsed properly
 */
TEST( ConfigFile, ExtraWhiteSpaces )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "  -t \n#foo=String \n -t\n\"@const bar=Int/&$\"  \n\n" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-f", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions = { "#foo=String", "@const bar=Int/&$" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
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
 * Check that escaped quotes are parsed properly
 */
TEST( ConfigFile, EscapedQuotes )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "  -t \n\"#foo=String~$ + \\\"bar\\\"\"" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "@", "-f", outputFilepath1.c_str() };

    std::vector<std::string> paramOverrideOptions;
    std::vector<std::string> typeOverrideOptions = { "#foo=String~$ + \\\"bar\\\"" };
    std::vector<std::string> includePaths;
    std::string outputText = "#####FOO#####";

    expect::Config$::Config$( false, paramOverrideOptions, typeOverrideOptions );
    expect::Parser$::Parse( IgnoreParameter::YES, inputFilename.c_str(), IgnoreParameter::YES, false, "", includePaths, &error, true );
    expect::Parser$::GenerateMock( IgnoreParameter::YES, "-t \"#foo=String~$ + \\\"bar\\\"\" ", &outputText );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 0, ret );
    STRCMP_EQUAL( outputText.c_str(), output.str().c_str() );
    CHECK_EQUAL( 0, error.tellp() );

    // Cleanup
}

/*
 * Check that if the config file has a non matching ending quote, an error is displayed
 */
TEST( ConfigFile, NotMatchingEndQuote )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );
    mock().installCopier( "std::ostream", stdOstreamCopier );

    std::ostringstream output;
    std::ostringstream error;
    App app( output, error );

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, " -t \n#foo=String \n -t \"@const bar=Int/&$  \n\n" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", inputFilename.c_str(), "-m", "-f", outputFilepath1.c_str() };

    expect::ConsoleColorizer$::SetColor( 2, IgnoreParameter::YES, IgnoreParameter::YES );

    // Exercise
    int ret = app.Execute( (int) args.size(), args.data() );

    // Verify
    CHECK_EQUAL( 1, ret );
    STRCMP_CONTAINS( "ERROR:", error.str().c_str() );
    STRCMP_CONTAINS( ( "In configuration file '" + outputFilepath1 + "':3:5 ending quote was not found" ).c_str(), error.str().c_str() );
    CHECK_EQUAL( 0, output.tellp() );

    // Cleanup
}
