cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Test.OutputFileParser )

# Test configuration

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set( PROD_SRC_FILES
     ${PROD_SOURCE_DIR}/sources/OutputFileParser.cpp
)

set( TEST_SRC_FILES
    OutputFileParser_test.cpp
)

#if( MSVC )
#    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
#endif()

# Generate test target

include( ${TEST_BASE_DIR}/GenerateTest.cmake )
