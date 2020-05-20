# The MIT License (MIT)
# 
# Copyright (c) 2015, by [halex2005](mailto:akharlov@gmail.com)
# Copyright (c) 2018-2020, by Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

include (CMakeParseArguments)

set( GenerateProductVersionTemplateDir "${CMAKE_CURRENT_LIST_DIR}/GenerateProductVersion" )

# generate_product_version() function
#
# This function uses VersionInfo.in.h and VersionResource.in.h template files, and 
# VersionResource.rc file to generate WIN32 resource with version information and 
# general resource strings.
#
# Usage:
#   generate_product_version(
#     SomeOutputResourceVariable
#     NAME MyGreatProject
#     ICON ${PATH_TO_APP_ICON}
#     VERSION_MAJOR 2
#     VERSION_MINOR 3
#     VERSION_BUILD ${BUILD_COUNTER}
#   )
# where BUILD_COUNTER could a value from your CI server.
#
# You can use generated resource for your executable targets:
#   add_executable(target-name ${target-files} ${SomeOutputResourceVariable})
#
# You can specify resource strings in arguments:
#   NAME               - name of executable (no defaults, ex: Microsoft Word)
#   BUNDLE             - bundle (${NAME} is default, ex: Microsoft Office)
#   ICON               - path to application icon (optional)
#   VERSION_MAJOR      - 1 is default
#   VERSION_MINOR      - 0 is default
#   VERSION_PATCH      - 0 is default
#   VERSION_BUILD      - 0 is default
#   COMPANY_NAME       - your company name (optional)
#   COMPANY_COPYRIGHT  - copyright (optional, (C) Copyright ${CURRENT_YEAR} ${COMPANY_NAME} is default
#                        if ${COMPANY_NAME} is defined)
#   COMMENTS           - ${NAME} v${VERSION_MAJOR}.${VERSION_MINOR} is default
#   ORIGINAL_FILENAME  - ${NAME} is default
#   INTERNAL_NAME      - ${NAME} is default
#   FILE_DESCRIPTION   - ${NAME} is default
#   PRIVATE_BUILD      - description of a private build (optional)
#   SPECIAL_BUILD      - description of a special build (optional)
#   PRERELEASE         - set to an expression that evaluates to true for prereleases (optional)
#   PATCHED            - set to an expression that evaluates to true for patched releases (optional)
function( generate_product_version outfiles )
    set( options
    	 EXPORT_TO_PARENT_SCOPE 
    )
    set( oneValueArgs
         NAME
         BUNDLE
         ICON
         VERSION_MAJOR
         VERSION_MINOR
         VERSION_PATCH
         VERSION_BUILD
         VERSION_SUFFIX
         COMPANY_NAME
         COMPANY_COPYRIGHT
         COMPANY_TRADEMARKS
         COMMENTS
         ORIGINAL_FILENAME
         INTERNAL_NAME
         FILE_DESCRIPTION
         PRIVATE_BUILD
         SPECIAL_BUILD
         PRERELEASE
         PATCHED
         HOMEPAGE
         MAINTAINER
    )
    set( multiValueArgs )
    cmake_parse_arguments( PRODUCT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if( NOT PRODUCT_BUNDLE )
        set( PRODUCT_BUNDLE "${PRODUCT_NAME}" )
    endif()
    
    if( PRODUCT_ICON )
        set( PRODUCT_ICON_FLAG "1" )
    else()
        set( PRODUCT_ICON_FLAG "0" )
    endif()

    if( NOT PRODUCT_VERSION_MAJOR )
        set( PRODUCT_VERSION_MAJOR 0 )
    endif()
    if( NOT PRODUCT_VERSION_MINOR )
        set( PRODUCT_VERSION_MINOR 0 )
    endif()
    if( NOT PRODUCT_VERSION_PATCH )
        set( PRODUCT_VERSION_PATCH 0 )
    endif()
    if( NOT PRODUCT_VERSION_BUILD )
        set( PRODUCT_VERSION_BUILD 0 )
    endif()
    
    if( PRODUCT_VERSION_PATCH GREATER 0 )
        set( PRODUCT_VERSION "${PRODUCT_VERSION_MAJOR}.${PRODUCT_VERSION_MINOR}.${PRODUCT_VERSION_PATCH}${PRODUCT_VERSION_SUFFIX}" )
    else()
        set( PRODUCT_VERSION "${PRODUCT_VERSION_MAJOR}.${PRODUCT_VERSION_MINOR}${PRODUCT_VERSION_SUFFIX}" )
    endif()

    if( NOT PRODUCT_COMPANY_COPYRIGHT AND PRODUCT_COMPANY_NAME )
        string( TIMESTAMP PRODUCT_CURRENT_YEAR "%Y" )
        set( PRODUCT_COMPANY_COPYRIGHT "(C) Copyright ${PRODUCT_CURRENT_YEAR} ${PRODUCT_COMPANY_NAME}")
    endif()
    
    if( NOT PRODUCT_COMMENTS )
        set( PRODUCT_COMMENTS "${PRODUCT_NAME} v${PRODUCT_VERSION}" )
    endif()
    
    if( NOT PRODUCT_ORIGINAL_FILENAME )
        set( PRODUCT_ORIGINAL_FILENAME "${PRODUCT_NAME}" )
    endif()
    
    if( NOT PRODUCT_INTERNAL_NAME )
        set( PRODUCT_INTERNAL_NAME "${PRODUCT_NAME}" )
    endif()
    
    if( NOT PRODUCT_FILE_DESCRIPTION )
        set( PRODUCT_FILE_DESCRIPTION "${PRODUCT_NAME}" )
    endif()
    
    if( PRODUCT_PRIVATE_BUILD )
        set( PRODUCT_PRIVATE_BUILD_FLAG "1" )
    else()
        set(PRODUCT_PRIVATE_BUILD_FLAG "0")
    endif()

    if( PRODUCT_SPECIAL_BUILD )
        set( PRODUCT_SPECIAL_BUILD_FLAG "1" )
    else()
        set( PRODUCT_SPECIAL_BUILD_FLAG "0" )
    endif()

    if( PRODUCT_PRERELEASE )
        set( PRODUCT_PRERELEASE_FLAG "1" )
    else()
        set( PRODUCT_PRERELEASE_FLAG "0" )
    endif()

    if( PRODUCT_PATCHED )
        set( PRODUCT_PATCHED_FLAG "1" )
    else()
        set( PRODUCT_PATCHED_FLAG "0" )
    endif()

    set( _VersionInfoHeaderFile ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.h )
    set( _VersionResourceHeaderFile ${CMAKE_CURRENT_BINARY_DIR}/VersionResource.h )
    set( _VersionResourceFile ${CMAKE_CURRENT_BINARY_DIR}/VersionResource.rc )
    configure_file(
        ${GenerateProductVersionTemplateDir}/VersionInfo.in.h
        ${_VersionInfoHeaderFile}
        @ONLY )
    configure_file(
        ${GenerateProductVersionTemplateDir}/VersionResource.in.h
        ${_VersionResourceHeaderFile}
        @ONLY )
    configure_file(
        ${GenerateProductVersionTemplateDir}/VersionResource.rc
        ${_VersionResourceFile}
        COPYONLY )
    list( APPEND ${outfiles} ${_VersionInfoHeaderFile} ${_VersionResourceHeaderFile} ${_VersionResourceFile} )
    set( ${outfiles} ${${outfiles}} PARENT_SCOPE )

    if( PRODUCT_EXPORT_TO_PARENT_SCOPE )
        set( PRODUCT_NAME ${PRODUCT_NAME} PARENT_SCOPE )
        set( PRODUCT_BUNDLE ${PRODUCT_BUNDLE} PARENT_SCOPE )
        set( PRODUCT_ICON ${PRODUCT_ICON} PARENT_SCOPE )
        set( PRODUCT_VERSION_MAJOR ${PRODUCT_VERSION_MAJOR} PARENT_SCOPE )
        set( PRODUCT_VERSION_MINOR ${PRODUCT_VERSION_MINOR} PARENT_SCOPE )
        set( PRODUCT_VERSION_PATCH ${PRODUCT_VERSION_PATCH} PARENT_SCOPE )
        set( PRODUCT_VERSION_BUILD ${PRODUCT_VERSION_BUILD} PARENT_SCOPE )
        set( PRODUCT_VERSION_SUFFIX ${PRODUCT_VERSION_SUFFIX} PARENT_SCOPE )
        set( PRODUCT_VERSION ${PRODUCT_VERSION} PARENT_SCOPE )
        set( PRODUCT_COMPANY_NAME ${PRODUCT_COMPANY_NAME} PARENT_SCOPE )
        set( PRODUCT_COMPANY_COPYRIGHT ${PRODUCT_COMPANY_COPYRIGHT} PARENT_SCOPE )
        set( PRODUCT_COMMENTS ${PRODUCT_COMMENTS} PARENT_SCOPE )
        set( PRODUCT_ORIGINAL_FILENAME ${PRODUCT_ORIGINAL_FILENAME} PARENT_SCOPE )
        set( PRODUCT_INTERNAL_NAME ${PRODUCT_INTERNAL_NAME} PARENT_SCOPE )
        set( PRODUCT_FILE_DESCRIPTION ${PRODUCT_FILE_DESCRIPTION} PARENT_SCOPE )
        set( PRODUCT_PRIVATE_BUILD ${PRODUCT_PRIVATE_BUILD} PARENT_SCOPE )
        set( PRODUCT_SPECIAL_BUILD ${PRODUCT_SPECIAL_BUILD} PARENT_SCOPE )
        set( PRODUCT_PRERELEASE ${PRODUCT_PRERELEASE} PARENT_SCOPE )
        set( PRODUCT_PATCHED ${PRODUCT_PATCHED} PARENT_SCOPE )
        set( PRODUCT_HOMEPAGE ${PRODUCT_HOMEPAGE} PARENT_SCOPE )
        set( PRODUCT_MAINTAINER ${PRODUCT_MAINTAINER} PARENT_SCOPE )
    endif()
endfunction()
