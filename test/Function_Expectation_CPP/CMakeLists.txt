cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Function_Expectation_CPP )

# Test configuration

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/Function.cpp
     ${PROD_SOURCE_DIR}/sources/ClangHelper.cpp
)

set( TEST_SRC_FILES
     Function_Expectation_test.cpp
     ${HELPERS_DIR}/ClangParseHelper.cpp
     ${HELPERS_DIR}/ClangCompileHelper.cpp
     ${MOCKS_DIR}/Config_mock.cpp
     ${EXPECTATIONS_DIR}/Config_expect.cpp
)

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
