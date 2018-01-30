cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen )
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 1 )
set( PROJECT_VERSION_PATCH 0 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

find_package( LibClang REQUIRED )

if( MSVC )
    include( ${CMAKE_SOURCE_DIR}/cmake/VSHelper.cmake )

    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} sources )
#    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} include )
endif( MSVC )

include_directories( ${LibClang_INCLUDE_DIRS}  ${CMAKE_CURRENT_SOURCE_DIR}/libs/cxxopts/include )
link_directories( ${LibClang_LIBRARY_DIRS} )

include( CheckFunctionExists )

if( WIN32 )
    set( PLATFORM_DIR "msw" )
elseif( UNIX )
    set( PLATFORM_DIR "linux" )
endif()

set( SRC_LIST
     sources/main.cpp
     sources/MockGenerator.cpp
     sources/ClangHelper.cpp
     sources/Function.cpp
     sources/Method.cpp
     sources/ConsoleColorizer.cpp
     sources/Config.cpp
)

set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )

include( ${CMAKE_SOURCE_DIR}/cmake/generate_product_version.cmake )

generate_product_version(
    PRODUCT_VERSION_FILES
    NAME ${PROJECT_NAME}
    VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
    VERSION_MINOR ${PROJECT_VERSION_MINOR}
    VERSION_PATCH ${PROJECT_VERSION_PATCH}
    COMPANY_NAME "www.gdr-sistemas.com"
    COMPANY_COPYRIGHT "(C) Copyright 2017-2018 Jesus Gonzalez"
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc" )
endif()

add_executable( ${PROJECT_NAME} ${SRC_LIST} ${PRODUCT_VERSION_FILES} )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 1 )

set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_dbg" )
set_target_properties( ${PROJECT_NAME} PROPERTIES COVERAGE_POSTFIX "_cov" )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

target_link_libraries( ${PROJECT_NAME} ${LibClang_LIBRARIES} )

add_dependencies( build ${PROJECT_NAME} )
