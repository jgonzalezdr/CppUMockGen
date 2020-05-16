cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Function_Mock_C )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

add_definitions( -DINTERPRET_C )

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
     ../Function_Mock_CPP/Function_Mock_test.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangParseHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/TestHelpers/ClangCompileHelper.cpp
     ${CMAKE_SOURCE_DIR}/test/Mocks/Config_mock.cpp
     ${CMAKE_SOURCE_DIR}/test/Expectations/Config_expect.cpp
)

if( NOT WIN32 )
    add_definitions( -DAVOID_LIBCLANG_MEMLEAK )
endif()

# Generate test target

include( ../GenerateTest.cmake )
