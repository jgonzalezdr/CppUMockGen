cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.ConfigFile )

# Test configuration

include_directories(
    ${PROD_SOURCE_DIR}/libs/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/App.cpp
     ${PROD_SOURCE_DIR}/sources/FileHelper.cpp
     ${PROD_SOURCE_DIR}/sources/ConfigFile.cpp
)

set( TEST_SRC_FILES
     ConfigFile_test.cpp
     ${MOCKS_DIR}/Parser_mock.cpp
     ${EXPECTATIONS_DIR}/Parser_expect.cpp
     ${MOCKS_DIR}/ConsoleColorizer_mock.cpp
     ${EXPECTATIONS_DIR}/ConsoleColorizer_expect.cpp
     ${MOCKS_DIR}/Config_mock.cpp
     ${EXPECTATIONS_DIR}/Config_expect.cpp
)

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
