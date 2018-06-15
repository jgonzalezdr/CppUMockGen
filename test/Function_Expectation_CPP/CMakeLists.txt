cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Function_Expectation_CPP )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
    ${CMAKE_SOURCE_DIR}/app/include/
    ${CMAKE_SOURCE_DIR}/test/Expectations/
)

set( PROD_SRC_FILES
     ${PROD_DIR}/sources/Function.cpp
     ${PROD_DIR}/sources/ClangHelper.cpp
)

set( TEST_SRC_FILES
     Function_Expectation_test.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangParseHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangCompileHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Config_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/Config_expect.cpp
)

# Generate test target

include( ../GenerateTest.cmake )
