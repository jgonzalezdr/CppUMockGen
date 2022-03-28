/**
 * @file
 * @brief      Unit tests for the "Options" class
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
#include <cxxopts.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "Options.hpp"

#include <filesystem>
#include <fstream>

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

static const std::filesystem::path tempDirPath = std::filesystem::temp_directory_path();

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

TEST_GROUP( OptionsFromCommandLine )
{
};

TEST_GROUP( OptionsFromString )
{
};

TEST_GROUP( OptionsFromConfigFile )
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

    void SetupTempFile( const std::filesystem::path &filepath, const SimpleString &contents )
    {
        std::ofstream tempFile;
        tempFile.open( filepath );
        tempFile << contents.asCharString();
        tempFile.close();
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

TEST( OptionsFromCommandLine, None )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Help_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-h" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_TRUE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );

    STRCMP_CONTAINS( "FooBar", options.GetHelpText().c_str() );
    STRCMP_CONTAINS( "Usage:\n  CppUMockGenFoo [OPTION...] <input>", options.GetHelpText().c_str() );
}

TEST( OptionsFromCommandLine, Help_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--help" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_TRUE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );

    STRCMP_CONTAINS( "FooBar", options.GetHelpText().c_str() );
    STRCMP_CONTAINS( "Usage:\n  CppUMockGenFoo [OPTION...] <input>", options.GetHelpText().c_str() );
}

TEST( OptionsFromCommandLine, Version_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-v" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_TRUE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Version_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-v" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_TRUE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Input_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-i", "INPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT_PATH", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Input_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--input", "INPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT_PATH", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Input_Positional )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "INPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT_PATH", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Mock_Short_NoPath )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-m" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Mock_Short_WithPath )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-m", "OUTPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "OUTPUT_PATH", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Mock_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--mock-output", "OUTPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "OUTPUT_PATH", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Expectations_Short_NoPath )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-e" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Expectations_Short_WithPath )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-e", "OUTPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "OUTPUT_PATH", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Expectations_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--expect-output", "OUTPUT_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "OUTPUT_PATH", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, BaseDirectory_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-B", "BASE_DIR_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE_DIR_PATH", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, BaseDirectory_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--base-directory", "BASE_DIR_PATH" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE_DIR_PATH", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Regeneration_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-r" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_TRUE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Regeneration_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--regen" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_TRUE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, InterpretAsCpp_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-x" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-x", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, InterpretAsCpp_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--cpp" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-x", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, LanguageStandard_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-s", "LANG_STD" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "LANG_STD", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-s LANG_STD", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, LanguageStandard_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--std", "LANG_STD" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "LANG_STD", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-s LANG_STD", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, UseUnderlyingTypedef_Short )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-u" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-u", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, UseUnderlyingTypedef_Long )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--underlying-typedef" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-u", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, IncludePaths )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-I", "IncludePath1", "--include-path", "IncludePath2" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 2, includePaths.size() );
    STRCMP_EQUAL( "IncludePath1", includePaths[0].c_str() );
    STRCMP_EQUAL( "IncludePath2", includePaths[1].c_str() );
}

TEST( OptionsFromCommandLine, TypeOverrides )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-t", "TypeOverride1", "--type-override", "TypeOverride2" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "-t TypeOverride1 -t TypeOverride2", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "TypeOverride1", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "TypeOverride2", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromCommandLine, Combination )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "-I", "IncludePath1", "-t", "@std::string=String", "--include-path", "IncludePath2",
            "--underlying-typedef", "--type-override", "#p1=String", "--cpp", "-B", "BASE DIR", "-m", "-e", "OUTPUT PATH", "-r",
            "-s", "c++11!", "INPUT FILE" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT FILE", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "OUTPUT PATH", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE DIR", options.GetBaseDirectory().c_str() );
    CHECK_TRUE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "c++11!", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-x", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-s \"c++11!\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-t \"@std::string=String\" -t \"#p1=String\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-u", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "@std::string=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "#p1=String", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 2, includePaths.size() );
    STRCMP_EQUAL( "IncludePath1", includePaths[0].c_str() );
    STRCMP_EQUAL( "IncludePath2", includePaths[1].c_str() );
}

TEST( OptionsFromCommandLine, NonExistingOption )
{
    // Prepare
    std::vector<const char *> args = { "CppUMockGen.exe", "--non-existing" };
    bool exceptionThrown = false;
    std::string exceptionMessage;

    Options options;

    // Exercise
    try
    {
        options.Parse( (int) args.size(), args.data() );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_CONTAINS( "Option 'non-existing' does not exist", exceptionMessage.c_str() );
}

TEST( OptionsFromString, Empty )
{
    // Prepare
    Options options;

    // Exercise
    options.Parse( "" );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromString, Combination )
{
    // Prepare
    Options options;

    // Exercise
    options.Parse( "  -I \"Include Path 1\" -t @std::string=String --include-path IncludePath2 "
            "--underlying-typedef --type-override #p1=String --cpp -B \"BASE DIR\" -e -m \"OUTPUT PATH\" -r "
            "-s c++11! -i \"INPUT FILE\"       " );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT FILE", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "OUTPUT PATH", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE DIR", options.GetBaseDirectory().c_str() );
    CHECK_TRUE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "c++11!", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-x", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-s \"c++11!\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-t \"@std::string=String\" -t \"#p1=String\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-u", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "@std::string=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "#p1=String", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 2, includePaths.size() );
    STRCMP_EQUAL( "Include Path 1", includePaths[0].c_str() );
    STRCMP_EQUAL( "IncludePath2", includePaths[1].c_str() );
}

TEST( OptionsFromString, NonExistingOption )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    Options options;

    // Exercise
    try
    {
        options.Parse( "--non-existing" );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_CONTAINS( "Option 'non-existing' does not exist", exceptionMessage.c_str() );
}

/*
 * Check that escaped quotes are parsed properly
 */
TEST( OptionsFromString, EscapedQuotes )
{
    // Prepare
    Options options;

    // Exercise
    options.Parse( "-t \"#foo=String~$ + \\\"bar\\\"\"" );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-t \"#foo=String~$ + \\\"bar\\\"\"", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 1, typeOverrideOptions.size() );
    STRCMP_EQUAL( "#foo=String~$ + \\\"bar\\\"", typeOverrideOptions[0].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_TRUE( includePaths.empty() );
}

/*
 * Check that if the config file has a non matching ending quote, an error is thrown
 */
TEST( OptionsFromString, NotMatchingEndQuote )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    Options options;

    // Exercise
    try
    {
        options.Parse( " -t \n#foo=String -t \"@const bar=Int/&$" );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_EQUAL( "In options ' -t \n#foo=String -t \"@const bar=Int/&$':21: Ending quote was not found.", exceptionMessage.c_str() );
}

 /*
  * Check that an empty configuration file is handled properly
  */
TEST( OptionsFromConfigFile, EmptyFile )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str() };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_EQUAL( "", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK( typeOverrideOptions.empty() );

    auto includePaths = options.GetIncludePaths();
    CHECK( includePaths.empty() );
}

TEST( OptionsFromConfigFile, Combination )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "  \n  -I \"Include Path 1\"  \n-t @std::string=String\n--include-path\nIncludePath2 "
            "--underlying-typedef --type-override #p1=String --cpp -B \"BASE DIR\"\n  -e   \n-m \"OUTPUT PATH\" -r\n"
            "-s c++11! -i \"INPUT FILE\"    \n   \n\n" );

    std::vector<const char *> args = { "CppUMockGen.exe", "--config-file", outputFilepath1.c_str() };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT FILE", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "OUTPUT PATH", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE DIR", options.GetBaseDirectory().c_str() );
    CHECK_TRUE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "c++11!", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-x", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-s \"c++11!\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-t \"@std::string=String\" -t \"#p1=String\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-u", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "@std::string=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "#p1=String", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 2, includePaths.size() );
    STRCMP_EQUAL( "Include Path 1", includePaths[0].c_str() );
    STRCMP_EQUAL( "IncludePath2", includePaths[1].c_str() );
}

TEST( OptionsFromConfigFile, NonExistingOption )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "--non-existing" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str() };

    Options options;

    // Exercise
    try
    {
        options.Parse( (int) args.size(), args.data() );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_CONTAINS( "Option 'non-existing' does not exist", exceptionMessage.c_str() );
}

/*
 * Check that if the config file cannot be opened, an error is thrown
 */
TEST( OptionsFromConfigFile, CannotOpenFile )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    std::string nonExistingFile = ( tempDirPath / "NonExistantDirectory123898876354874" / "ConfigFile.cfg" ).generic_string();

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", nonExistingFile.c_str() };

    Options options;

    // Exercise
    try
    {
        options.Parse( (int) args.size(), args.data() );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_EQUAL( ("Configuration file '" + nonExistingFile + "' could not be opened.").c_str(), exceptionMessage.c_str() );
}

/*
 * Check that other config files specified in a config file are parsed properly
 */
TEST( OptionsFromConfigFile, IncludeOtherConfigFile_1Level )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 -I IncludePath2\n-f NestedDir/NestedConfigFile.cfg" );

    std::filesystem::path nestedDirPath = tempDirPath / "NestedDir";
    std::filesystem::create_directory( nestedDirPath );

    outputFilepath2 = ( nestedDirPath / "NestedConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath2, "-t foo#bar=String\n-t foo@=Int/&$ --cpp\n  --base-directory BASE_DIR" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str() };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "BASE_DIR", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_TRUE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-x", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-t \"foo#bar=String\" -t \"foo@=Int/&$\"", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "foo#bar=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "foo@=Int/&$", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 2, includePaths.size() );
    STRCMP_EQUAL( "IncludePath1", includePaths[0].c_str() );
    STRCMP_EQUAL( "IncludePath2", includePaths[1].c_str() );

    // Cleanup
    std::filesystem::remove_all( nestedDirPath );
    outputFilepath2.clear();
}

/*
 * Check that other config files specified in a config file are parsed properly
 */
TEST( OptionsFromConfigFile, IncludeOtherConfigFile_2Level )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-I IncludePath1 \n-f NestedDir/NestedConfigFile.cfg" );

    std::filesystem::path nestedDirPath = tempDirPath / "NestedDir";
    std::filesystem::create_directory( nestedDirPath );

    outputFilepath2 = ( nestedDirPath / "NestedConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath2, "-s gnu++9323 -t foo#bar=String\n-f ../ConfigFile2.cfg\n-t foo@=Int/&$" );

    outputFilepath3 = ( tempDirPath / "ConfigFile2.cfg" ).generic_string();
    SetupTempFile( outputFilepath3, "-u -t\n#foo=String\n-t\n\"@const bar=Int/&$\"" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-i", "INPUT FILE", "-f", outputFilepath1.c_str(), "-m", "@" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "INPUT FILE", options.GetInputPath().c_str() );
    CHECK_TRUE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "@", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "gnu++9323", options.GetLanguageStandard().c_str() );
    CHECK_TRUE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-u", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-t \"foo#bar=String\" -t \"foo@=Int/&$\" -t \"#foo=String\" -t \"@const bar=Int/&$\"", options.GetGenerationOptions().c_str() );
    STRCMP_CONTAINS( "-s gnu++9323", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 4, typeOverrideOptions.size() );
    STRCMP_EQUAL( "foo#bar=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "foo@=Int/&$", typeOverrideOptions[1].c_str() );
    STRCMP_EQUAL( "#foo=String", typeOverrideOptions[2].c_str() );
    STRCMP_EQUAL( "@const bar=Int/&$", typeOverrideOptions[3].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_EQUAL( 1, includePaths.size() );
    STRCMP_EQUAL( "IncludePath1", includePaths[0].c_str() );

    // Cleanup
    std::filesystem::remove_all( nestedDirPath );
    outputFilepath2.clear();
}

/*
 * Check that if an included config file cannot be opened, an error is thrown
 */
TEST( OptionsFromConfigFile, IncludeOtherConfigFile_CannotOpenFile )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    std::string nonExistingFile = ( tempDirPath / "NonExistantDirectory123898876354874" / "ConfigFile.cfg" ).generic_string();

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, ("-I IncludePath1 -I IncludePath2\n-f " + nonExistingFile).c_str() );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str(), "-m", "@" };

    Options options;

    // Exercise
    try
    {
        options.Parse( (int) args.size(), args.data() );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_EQUAL( ("Configuration file '" + nonExistingFile + "' could not be opened.").c_str(), exceptionMessage.c_str() );
}

/*
 * Check that if a config file is included recursively, it is ignored
 */
TEST( OptionsFromConfigFile, IncludeOtherConfigFile_Recursive )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-i \"Input File\"\n-f ConfigFile.cfg\n-t foo#bar=String\n-t foo@=Int/&$" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str(), "-e", "@" };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "Input File", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_TRUE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "@", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-t \"foo#bar=String\" -t \"foo@=Int/&$\"", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 2, typeOverrideOptions.size() );
    STRCMP_EQUAL( "foo#bar=String", typeOverrideOptions[0].c_str() );
    STRCMP_EQUAL( "foo@=Int/&$", typeOverrideOptions[1].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_TRUE( includePaths.empty() );
}

/*
 * Check that escaped quotes are parsed properly
 */
TEST( OptionsFromConfigFile, EscapedQuotes )
{
    // Prepare
    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, "-t \n\"#foo=String~$ + \\\"bar\\\"\"" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str() };

    Options options;

    // Exercise
    options.Parse( (int) args.size(), args.data() );

    // Verify
    CHECK_FALSE( options.IsHelpRequested() );
    CHECK_FALSE( options.IsVersionRequested() );
    STRCMP_EQUAL( "", options.GetInputPath().c_str() );
    CHECK_FALSE( options.IsMockRequested() );
    CHECK_FALSE( options.IsExpectationsRequested() );
    STRCMP_EQUAL( "", options.GetMockOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetExpectationsOutputPath().c_str() );
    STRCMP_EQUAL( "", options.GetBaseDirectory().c_str() );
    CHECK_FALSE( options.IsRegenerationRequested() );
    CHECK_FALSE( options.InterpretAsCpp() );
    STRCMP_EQUAL( "", options.GetLanguageStandard().c_str() );
    CHECK_FALSE( options.UseUnderlyingTypedef() );
    STRCMP_CONTAINS( "-t \"#foo=String~$ + \\\"bar\\\"\"", options.GetGenerationOptions().c_str() );

    auto typeOverrideOptions = options.GetTypeOverrides();
    CHECK_EQUAL( 1, typeOverrideOptions.size() );
    STRCMP_EQUAL( "#foo=String~$ + \\\"bar\\\"", typeOverrideOptions[0].c_str() );

    auto includePaths = options.GetIncludePaths();
    CHECK_TRUE( includePaths.empty() );
}

/*
 * Check that if the config file has a non matching ending quote, an error is thrown
 */
TEST( OptionsFromConfigFile, NotMatchingEndQuote )
{
    // Prepare
    bool exceptionThrown = false;
    std::string exceptionMessage;

    outputFilepath1 = ( tempDirPath / "ConfigFile.cfg" ).generic_string();
    SetupTempFile( outputFilepath1, " -t \n#foo=String \n -t \"@const bar=Int/&$  \n\n" );

    std::vector<const char *> args = { "CppUMockGen.exe", "-f", outputFilepath1.c_str() };

    Options options;

    // Exercise
    try
    {
        options.Parse( (int) args.size(), args.data() );
    }
    catch( const std::exception& e )
    {
        exceptionThrown = true;
        exceptionMessage = e.what();
    }

    // Verify
    CHECK_TRUE( exceptionThrown );
    STRCMP_EQUAL( ( "In configuration file '" + outputFilepath1 + "':3:5: Ending quote was not found." ).c_str(), exceptionMessage.c_str() );
}
