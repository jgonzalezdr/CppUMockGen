cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Parser )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set( PROD_SRC_FILES
     ${PROD_DIR}/sources/Parser.cpp
     ${PROD_DIR}/sources/ClangHelper.cpp
     ${PROD_DIR}/sources/FileHelper.cpp
)

set( TEST_SRC_FILES
     Parser_test.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Config_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Function_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Method_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/ConsoleColorizer_mock.cpp
)

if( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
endif()

# Generate test target

include( ../GenerateTest.cmake )
