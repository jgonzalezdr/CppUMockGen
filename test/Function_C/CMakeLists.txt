cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.Test.Function_C )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

add_definitions( -DINTERPRET_C )

include_directories(
    ${PROD_DIR}/sources
)

set( PROD_SRC_FILES 
     ${PROD_DIR}/sources/Function.cpp
     ${PROD_DIR}/sources/ClangHelper.cpp
)

set( TEST_SRC_FILES 
     ../Function_CPP/Function_test.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangParseHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangCompileHelper.cpp
)

# Generate test target

include( ../GenerateTest.cmake )
