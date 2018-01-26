cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.AllTests )

option( CHECK_COMPILATION "Check that generated code compiles properly" ON )

enable_testing()

include( CommonTestTargets.cmake )

if( NOT CHECK_COMPILATION )
    add_definitions( -DDISABLE_COMPILATION_CHECK )
endif()

# Test modules

add_subdirectory( Function_CPP )
add_subdirectory( Function_C )
add_subdirectory( Method )
add_subdirectory( Config )
