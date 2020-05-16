cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Parser )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/app/include/
    ${CMAKE_SOURCE_DIR}/test/Expectations/
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
     ${CMAKE_SOURCE_DIR}/test/Expectations/Function_expect.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/ConsoleColorizer_expect.cpp
)

if( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
endif()

string( REPLACE \\ / C_PROD_DIR ${PROD_DIR} )
add_definitions( -DPROD_DIR="${C_PROD_DIR}" )

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ../GenerateTest.cmake )
