cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Constructor_Expectation )

# Test configuration

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/Function.cpp
     ${PROD_SOURCE_DIR}/sources/Constructor.cpp
     ${PROD_SOURCE_DIR}/sources/ClangHelper.cpp
     ${PROD_SOURCE_DIR}/sources/StringHelper.cpp
)

set( TEST_SRC_FILES
     Constructor_Expectation_test.cpp
     ${HELPERS_DIR}/ClangParseHelper.cpp
     ${HELPERS_DIR}/ClangCompileHelper.cpp
     ${MOCKS_DIR}/Config_mock.cpp
)

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
