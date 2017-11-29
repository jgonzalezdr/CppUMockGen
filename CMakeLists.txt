cmake_minimum_required( VERSION 3.1 )

project( CppUMockGen.Top )

option( COVERAGE "Enable coverage analysis" ON )

if( COVERAGE AND NOT MSVC )

    set( CMAKE_CONFIGURATION_TYPES "Debug;Release;Coverage" CACHE STRING "" FORCE )

else()

    set( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE )

endif()

if( WIN32 AND NOT MSVC )
    add_definitions( -DWIN32 )
endif()

add_custom_target( build )

add_subdirectory( app )
add_subdirectory( test )
