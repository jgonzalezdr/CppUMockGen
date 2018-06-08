cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.AllTests )

option( CHECK_COMPILATION "Check that generated code compiles properly" ON )

enable_testing()

include( CommonTestTargets.cmake )

if( NOT CHECK_COMPILATION )
    add_definitions( -DDISABLE_COMPILATION_CHECK )
endif()

# Test modules

add_subdirectory( App )
add_subdirectory( Parser )
add_subdirectory( Config )
add_subdirectory( Function_Mock_CPP )
add_subdirectory( Function_Mock_C )
add_subdirectory( Function_Expectation_CPP )
add_subdirectory( Function_Expectation_C )
add_subdirectory( Method_Mock )
add_subdirectory( Method_Expectation )
