cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Method_Expectation )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
)

set( PROD_SRC_FILES
     ${PROD_DIR}/sources/Function.cpp
     ${PROD_DIR}/sources/Method.cpp
     ${PROD_DIR}/sources/ClangHelper.cpp
)

set( TEST_SRC_FILES
     Method_Expectation_test.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangParseHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangCompileHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Config_mock.cpp
)

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ../GenerateTest.cmake )
