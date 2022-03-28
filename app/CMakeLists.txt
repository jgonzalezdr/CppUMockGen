cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen VERSION 0.6.0 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

find_package( LibClang REQUIRED )

if( MSVC )
    include( VisualStudioHelper )

    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} sources )
    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} include )
endif( MSVC )

set( SRC_LIST
     sources/main.cpp
     sources/App.cpp
     sources/Parser.cpp
     sources/ClangHelper.cpp
     sources/Function.cpp
     sources/Method.cpp
     sources/Constructor.cpp
     sources/Destructor.cpp
     sources/ConsoleColorizer.cpp
     sources/Config.cpp
     sources/FileHelper.cpp
     sources/Options.cpp
     sources/StringHelper.cpp
     sources/OutputFileParser.cpp
)

set( INC_LIST
     sources/App.hpp
     sources/Parser.hpp
     sources/ClangHelper.hpp
     sources/Function.hpp
     sources/Method.hpp
     sources/Constructor.hpp
     sources/Destructor.hpp
     sources/ConsoleColorizer.hpp
     sources/Config.hpp
     sources/FileHelper.hpp
     sources/Options.hpp
     sources/StringHelper.hpp
     sources/OutputFileParser.hpp
     include/CppUMockGen.hpp
)

#
# Project information
#

if( NOT PROJECT_VERSION_BUILD )
    set( PROJECT_VERSION_BUILD ${PROJECT_VERSION_TWEAK} )
endif()

include( GenerateProductVersion )

generate_product_version(
    PRODUCT_VERSION_FILES
    NAME ${PROJECT_NAME}
    FILE_DESCRIPTION "Mock generator for CppUTest"
    VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
    VERSION_MINOR ${PROJECT_VERSION_MINOR}
    VERSION_PATCH ${PROJECT_VERSION_PATCH}
    VERSION_BUILD ${PROJECT_VERSION_BUILD}
    VERSION_SUFFIX "${PROJECT_VERSION_SUFFIX}"
    COMPANY_NAME "www.gdr-sistemas.com"
    COMPANY_COPYRIGHT "(C) Copyright 2017-2022 Jesus Gonzalez"
    PRIVATE_BUILD "${PRIVATE_BUILD_INFO}"
    SPECIAL_BUILD "${SPECIAL_BUILD_INFO}"
    PRERELEASE "${PRERELEASE}"
    PATCHED "${PATCHED_RELEASE}"
    HOMEPAGE "https://github.com/jgonzalezdr/CppUMockGen"
    MAINTAINER "Jesus Gonzalez <jgonzalez@gdr-sistemas.com>"
    EXPORT_TO_PARENT_SCOPE
)

if( WIN32 AND ( LibClang_TYPE STREQUAL "SHARED_LIBRARY" ) )
    get_target_property( LibClang_DLL_PATH LibClang IMPORTED_LOCATION )
endif()

#
# Executable definition
#

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc" )
endif()

add_executable( ${PROJECT_NAME} ${SRC_LIST} ${INC_LIST} ${PRODUCT_VERSION_FILES} )

if( LibClang_DLL_PATH )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LibClang_DLL_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME}> )
endif()

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )

add_dependencies( build ${PROJECT_NAME} )

#
# C++ standard
#

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 1 )

#
# External libraries
#

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/cxxopts/include )

target_link_libraries( ${PROJECT_NAME} ${LibClang_LIBRARIES} )

if( (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0") )
    target_link_libraries( ${PROJECT_NAME} stdc++fs )
endif()

#include_directories( ${LibClang_INCLUDE_DIR} )
#link_directories( ${LibClang_LIBRARY_DIR} )

#
# Executable properties
#

set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_dbg" )
set_target_properties( ${PROJECT_NAME} PROPERTIES COVERAGE_POSTFIX "_cov" )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

#
# Install
#

include( InstallersSetup.cmake )

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
install( DIRECTORY include/ DESTINATION include )

get_target_property( LibClang_TYPE LibClang TYPE )
if( LibClang_DLL_PATH )
    install( FILES ${LibClang_DLL_PATH} DESTINATION bin )
endif()
