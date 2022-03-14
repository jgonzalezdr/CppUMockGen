cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Parser )

# Test configuration

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/Parser.cpp
     ${PROD_SOURCE_DIR}/sources/ClangHelper.cpp
     ${PROD_SOURCE_DIR}/sources/FileHelper.cpp
     ${PROD_SOURCE_DIR}/sources/StringHelper.cpp
)

set( TEST_SRC_FILES
     Parser_test.cpp
     ${MOCKS_DIR}/Config_mock.cpp
     ${EXPECTATIONS_DIR}/Config_expect.cpp
     ${MOCKS_DIR}/Function_mock.cpp
     ${EXPECTATIONS_DIR}/Function_expect.cpp
     ${MOCKS_DIR}/Method_mock.cpp
     ${EXPECTATIONS_DIR}/Method_expect.cpp
     ${MOCKS_DIR}/Constructor_mock.cpp
     ${EXPECTATIONS_DIR}/Constructor_expect.cpp
     ${MOCKS_DIR}/Destructor_mock.cpp
     ${EXPECTATIONS_DIR}/Destructor_expect.cpp
     ${MOCKS_DIR}/ConsoleColorizer_mock.cpp
     ${EXPECTATIONS_DIR}/ConsoleColorizer_expect.cpp
)

if( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
endif()

string( REPLACE \\ / C_PROD_DIR ${PROD_SOURCE_DIR} )
add_definitions( -DPROD_DIR="${C_PROD_DIR}" )

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
