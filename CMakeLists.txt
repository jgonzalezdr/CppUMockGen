cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Top )

option( ENABLE_TEST "Enable building tests" ON )
option( CI_MODE "Enable continuous integration mode" OFF )

if( CMAKE_CONFIGURATION_TYPES )
    option( COVERAGE "Enable coverage analysis" ON )
else()
    set( DEFAULT_BUILD_TYPE "Release" )

    set( BUILD_TYPES_LIST "Debug" "Release" "RelWithDebInfo" "MinSizeRel" "Coverage" )

    string( REPLACE ";" " " BUILD_TYPES_STRLIST "${BUILD_TYPES_LIST}" )
    
    if( NOT CMAKE_BUILD_TYPE )
        message( STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified" )
        set( CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: ${BUILD_TYPES_STRLIST}" FORCE )
    else()
        set( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: ${BUILD_TYPES_STRLIST}" FORCE )
    endif()
    
    if( NOT CMAKE_BUILD_TYPE IN_LIST BUILD_TYPES_LIST )
        message( FATAL_ERROR "Unknown build type '${CMAKE_BUILD_TYPE}', please choose a supported build type: ${BUILD_TYPES_STRLIST}" )
    endif()

    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES_LIST} )
    
    if( CMAKE_BUILD_TYPE STREQUAL "Coverage" )
        set( COVERAGE ON )
    else()
        set( COVERAGE OFF )
    endif()
    
    set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG" )
    set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG" )
    set( CMAKE_RC_FLAGS_DEBUG "${CMAKE_RC_FLAGS_DEBUG} -D_DEBUG" )

    set( CMAKE_CXX_FLAGS_COVERAGE ${CMAKE_CXX_FLAGS_DEBUG} )
    set( CMAKE_C_FLAGS_COVERAGE ${CMAKE_C_FLAGS_DEBUG} )
    set( CMAKE_RC_FLAGS_COVERAGE ${CMAKE_RC_FLAGS_DEBUG} )
    set( CMAKE_EXE_LINKER_FLAGS_COVERAGE ${CMAKE_EXE_LINKER_FLAGS_DEBUG} )
endif()

if( WIN32 AND NOT MSVC )
    add_definitions( -DWIN32 )
endif()

add_custom_target( build )

add_subdirectory( app )

if( ENABLE_TEST )
    add_subdirectory( test )
endif()
