cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.Test.Config )

# Test configuration

set( PROD_DIR ${CMAKE_SOURCE_DIR}/app )

include_directories(
    ${PROD_DIR}/sources
)

set( PROD_SRC_FILES 
     ${PROD_DIR}/sources/Config.cpp
)

set( TEST_SRC_FILES 
     Config_test.cpp
)

# Generate test target

include( ../GenerateTest.cmake )
