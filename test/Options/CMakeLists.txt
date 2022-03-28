cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Options )

# Test configuration

include_directories(
    ${PROD_SOURCE_DIR}/libs/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/Options.cpp
)

set( TEST_SRC_FILES
     Options_test.cpp
)

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
