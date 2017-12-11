cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.AllTests )

enable_testing()

include( CommonTestTargets.cmake )

# Test modules

add_subdirectory( Function )
