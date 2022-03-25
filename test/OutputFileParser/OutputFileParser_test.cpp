/**
 * @file
 * @brief      Unit tests for the "OutputFileParser" class
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

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>

#include "Subtest.hpp"

#include "OutputFileParser.hpp"

/*===========================================================================
 *                      COMMON TEST DEFINES & MACROS
 *===========================================================================*/

/*===========================================================================
 *                          TEST GROUP DEFINITION
 *===========================================================================*/

static const std::filesystem::path tempDirPath = std::filesystem::temp_directory_path();
static const std::string defaultTempFilename = "CppUMockGen_mock.cpp";
static const std::string nonexistingFilePath = ( tempDirPath / "CppUMockGen_mock_not_existing.cpp" ).generic_string();

TEST_GROUP( OutputFileParser )
{
    std::ofstream tempFile;
    std::filesystem::path initialDir;
    std::string tempFilePath;

    TEST_SETUP()
    {
        initialDir = std::filesystem::current_path();
    }

    TEST_TEARDOWN()
    {
        std::filesystem::current_path( initialDir );

        if( !tempFilePath.empty() )
        {
            std::filesystem::remove( tempFilePath );
        }
    }

    void SetupTempFile( const SimpleString& contents )
    {
        SetupTempFile( defaultTempFilename, contents );
    }

    void SetupTempFile( std::string filename, const SimpleString& contents )
    {
        auto tempPath = ( tempDirPath / filename );
        
        std::filesystem::create_directories( tempPath.parent_path() );
        std::filesystem::remove( tempPath );

        tempFile.open( tempPath );
        tempFile << contents.asCharString();
        tempFile.close();
        
        tempFilePath = tempPath.generic_string();
    }
};

/*===========================================================================
 *                    TEST CASES IMPLEMENTATION
 *===========================================================================*/

/*
 * Check that user code is obtained property from the file.
 */
TEST( OutputFileParser, UserCode_Simple )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "// " USER_CODE_BEGIN "\n"
                "This is the user code\n"
                "// " USER_CODE_END "\n";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "This is the user code\n", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that user code is obtained property from the file.
 */
TEST( OutputFileParser, UserCode_Complex )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "Does not matter\n"
                "  //   " USER_CODE_BEGIN "   \n"
                "This is the user code\n"
                "...spanning more...\n"
                "than one line\n"
                "# // " USER_CODE_END " !!!\n"
                "Should be ignored";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "This is the user code\n...spanning more...\nthan one line\n", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that a file without user code is handled properly.
 */
TEST( OutputFileParser, EmptyUserCode )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "Does not matter\n"
                "//" USER_CODE_BEGIN "\n"
                "//" USER_CODE_END "\n"
                "Should be ignored";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that a file without user code is handled properly.
 */
TEST( OutputFileParser, NoUserCode )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "Does not matter\n"
                "  // xUSER_CODE_BEGIN   \n"
                "This is not used code\n"
                "# // xUSER_CODE_END !!!\n"
                "Should be ignored";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that a file without user code begin mark is handled properly.
 */
TEST( OutputFileParser, NoUserCodeBeginMark )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "Does not matter\n"
                "  // xUSER_CODE_BEGIN   \n"
                "This should be ignored\n"
                "# // " USER_CODE_END " !!!\n"
                "This should also be ignored";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that a file without user code end mark is handled properly.
 */
TEST( OutputFileParser, NoUserCodeEndMark )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        SimpleString fileContents =
                "Does not matter\n"
                "  // " USER_CODE_BEGIN "   \n"
                "This should be ignored\n"
                "# // xUSER_CODE_END !!!\n"
                "This should also be ignored";
        SetupTempFile( fileContents );

        // Exercise
        outputFileParser->Parse( tempFilePath );

        // Verify
        STRCMP_EQUAL( "", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}

/*
 * Check that non-existant file is handled properly.
 */
TEST( OutputFileParser, NonExistingInputFile )
{
    SUBTEST_BEGIN( "OutputFileParser instance creation" )

        // Exercise
        OutputFileParser *outputFileParser = new OutputFileParser();

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser::Parse invocation" )

        // Prepare
        // Exercise
        outputFileParser->Parse( nonexistingFilePath );

        // Verify
        STRCMP_EQUAL( "", outputFileParser->GetUserCode().c_str() );

    SUBTEST_END

    SUBTEST_BEGIN( "OutputFileParser instance deletion" )

        // Exercise
        delete( outputFileParser );

    SUBTEST_END
}
