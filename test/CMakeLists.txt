cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.AllTests )

enable_testing()

#set( COVERAGE_EXCLUDED \"*/libs/*\" \"*/sources/CAL/*\" \"*/sources/EOL/CAL/*\" )

include( CommonTestTargets.cmake )

# Test modules

add_subdirectory( Function )
