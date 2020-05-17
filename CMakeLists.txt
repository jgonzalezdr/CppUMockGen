cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen.Top )

option( ENABLE_TEST "Enable building tests" ON )
option( CI_MODE "Enable continuous integration mode" OFF )

if( WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.2.0") )
    message( FATAL_ERROR "MinGW must be at least version 8.2.0" )
endif()
if( NOT WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0") )
    message( FATAL_ERROR "GCC must be at least version 8.0.0" )
endif()

if( CMAKE_CONFIGURATION_TYPES )
    option( COVERAGE "Enable coverage analysis" ON )
else()
    set( DEFAULT_BUILD_TYPE "Release" )

    set( BUILD_TYPES_LIST "Debug" "Release" "RelWithDebInfo" "MinSizeRel" "Coverage" )
	
	set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;Coverage" )
	
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
add_subdirectory( test )

set( CONFIG_MSG "
--------------------------------------------------------------------------
                            CONFIGURATION
--------------------------------------------------------------------------

Build Options:" )

if( DEFINED CMAKE_BUILD_TYPE )
set( CONFIG_MSG "${CONFIG_MSG}
    CMAKE_BUILD_TYPE:                   ${CMAKE_BUILD_TYPE}" )
endif()

set( CONFIG_MSG "${CONFIG_MSG}
    CI_MODE:                            ${CI_MODE}

Configured Features:
    ENABLE_TEST:                        ${ENABLE_TEST}
    COVERAGE:                           ${COVERAGE}
    COVERAGE_VERBOSE:                   ${COVERAGE_VERBOSE}
    CHECK_COMPILATION:                  ${CHECK_COMPILATION}

--------------------------------------------------------------------------
" )

message( STATUS ${CONFIG_MSG} )
