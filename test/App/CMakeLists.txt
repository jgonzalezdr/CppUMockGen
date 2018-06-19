cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.App )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROD_DIR}/libs/cxxopts/include
    ${CMAKE_SOURCE_DIR}/app/include/
    ${CMAKE_SOURCE_DIR}/test/Expectations/
)

set( PROD_SRC_FILES
     ${PROD_DIR}/sources/App.cpp
     ${PROD_DIR}/sources/FileHelper.cpp
)

set( TEST_SRC_FILES
     App_test.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Parser_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/ConsoleColorizer_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Config_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/Parser_expect.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/ConsoleColorizer_expect.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/Config_expect.cpp
)

# Generate test target

include( ../GenerateTest.cmake )
