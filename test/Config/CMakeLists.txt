cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.Config )

# Test configuration

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/Config.cpp
     ${PROD_SOURCE_DIR}/sources/StringHelper.cpp
)

set( TEST_SRC_FILES
     Config_test.cpp
)

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
