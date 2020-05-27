cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen VERSION 0.4.0 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

find_package( LibClang REQUIRED )

if( MSVC )
    include( VisualStudioHelper )

    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} sources )
    generate_groups( ${CMAKE_CURRENT_SOURCE_DIR} include )
endif( MSVC )

include_directories( ${LibClang_INCLUDE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/libs/cxxopts/include )
link_directories( ${LibClang_LIBRARY_DIR} )

include( CheckFunctionExists )

set( SRC_LIST
     sources/main.cpp
     sources/App.cpp
     sources/Parser.cpp
     sources/ClangHelper.cpp
     sources/Function.cpp
     sources/Method.cpp
     sources/ConsoleColorizer.cpp
     sources/Config.cpp
     sources/FileHelper.cpp
     sources/ConfigFile.cpp
     sources/StringHelper.cpp
)

set( INC_LIST
     sources/App.hpp
     sources/Parser.hpp
     sources/ClangHelper.hpp
     sources/Function.hpp
     sources/Method.hpp
     sources/ConsoleColorizer.hpp
     sources/Config.hpp
     sources/FileHelper.hpp
     sources/ConfigFile.hpp
     sources/StringHelper.hpp
     include/CppUMockGen.hpp
)

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
    COMPANY_COPYRIGHT "(C) Copyright 2017-2020 Jesus Gonzalez"
    PRIVATE_BUILD "${PRIVATE_BUILD_INFO}"
    SPECIAL_BUILD "${SPECIAL_BUILD_INFO}"
    PRERELEASE "${PRERELEASE}"
    PATCHED "${PATCHED_RELEASE}"
    HOMEPAGE "https://github.com/jgonzalezdr/CppUMockGen"
    MAINTAINER "Jesús González <jgonzalez@gdr-sistemas.com>"
    EXPORT_TO_PARENT_SCOPE
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc" )
endif()

add_executable( ${PROJECT_NAME} ${SRC_LIST} ${INC_LIST} ${PRODUCT_VERSION_FILES} )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 1 )

set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_dbg" )
set_target_properties( ${PROJECT_NAME} PROPERTIES COVERAGE_POSTFIX "_cov" )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

target_link_libraries( ${PROJECT_NAME} ${LibClang_LIBRARY} )

if( (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0") )
    target_link_libraries( ${PROJECT_NAME} stdc++fs )
endif()

add_dependencies( build ${PROJECT_NAME} )

#
# Install
#

function( string_escape out_var in_string )
    string( REPLACE "\\" "\\\\" tmp_string "${in_string}" )
    string( REPLACE "\$" "\\\$" tmp_string "${tmp_string}" )
    string( REPLACE "\"" "\\\"" tmp_string "${tmp_string}" )
    set( ${out_var} "${tmp_string}" PARENT_SCOPE )
endfunction()

set( CPACK_PACKAGE_NAME ${PRODUCT_NAME} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${PRODUCT_NAME} )
set( CPACK_PACKAGE_VENDOR ${PRODUCT_COMPANY_NAME} )
set( CPACK_PACKAGE_VERSION_MAJOR ${PRODUCT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${PRODUCT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${PRODUCT_VERSION_PATCH} )
set( CPACK_PACKAGE_VERSION ${PRODUCT_VERSION} )
#set( CPACK_PACKAGE_DESCRIPTION ${PRODUCT_FILE_DESCRIPTION} )
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/debian/package_description"  )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_COMMENTS} )
set( CPACK_PACKAGE_HOMEPAGE_URL ${PRODUCT_HOMEPAGE} )
set( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt )
set( CPACK_BINARIES_SUBDIRECTORY "bin" )

string( TOUPPER "${PRODUCT_NAME}_HOME" CPACK_HOME_ENVVAR )

if( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
    set( CPACK_PACKAGE_ARCH_BITSIZE "64" )
elseif( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )
    set( CPACK_PACKAGE_ARCH_BITSIZE "32" )
else()
    message( FATAL_ERROR "Could not determine architecture bitsize (32 or 64)" )
endif()

set( CPACK_SOURCE_IGNORE_FILES "/build" "/coverage" "\.git" "\.vs" "vscode\.code-workspace" "\.vscode" )

if( WIN32 )
    if( NOT DEFINED CPACK_GENERATOR )
      set( CPACK_GENERATOR NSIS ZIP )
    endif()

    if( NOT DEFINED CPACK_SOURCE_GENERATOR )
      set( CPACK_SOURCE_GENERATOR ZIP )
    endif()

    string_escape( CPACK_NSIS_MUI_ICON "\${NSISDIR}\\Contrib\\Graphics\\Icons\\nsis3-install.ico" )
    string_escape( CPACK_NSIS_MUI_UNIICON "\${NSISDIR}\\Contrib\\Graphics\\Icons\\nsis3-uninstall.ico" )
    string_escape( CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Wizard\\nsis3-metro.bmp" )
    string_escape( CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Wizard\\nsis3-grey.bmp" )
    string_escape( CPACK_NSIS_MUI_HEADERIMAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Header\\nsis3-metro.bmp" )
    string_escape( CPACK_NSIS_MUI_HEADERIMAGE_UNBITMAP "\${NSISDIR}\\Contrib\\Graphics\\Header\\nsis3-grey.bmp" )
    set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    set( CPACK_NSIS_MUI_FINISHPAGE_NOAUTOCLOSE "ON" )
    set( CPACK_NSIS_CONTACT ${PRODUCT_MAINTAINER} )
    set( CPACK_NSIS_URL_INFO_ABOUT ${PRODUCT_HOMEPAGE} )

    set( CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CPackGen.cmake" )
else()
    if( NOT DEFINED CPACK_GENERATOR )
      set( CPACK_GENERATOR DEB TXZ )
    endif()

    if( NOT DEFINED CPACK_SOURCE_GENERATOR )
      set( CPACK_SOURCE_GENERATOR TXZ )
    endif()

    set( CPACK_DEBIAN_PACKAGE_MAINTAINER ${PRODUCT_MAINTAINER} )
    set( CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PRODUCT_HOMEPAGE} )
    set( CPACK_DEBIAN_FILE_NAME DEB-DEFAULT )
    #set( CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION} )
    set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
    set( CPACK_DEBIAN_PACKAGE_RECOMMENDS "cpputest" )
    if( NOT PRODUCT_PRERELEASE )
        set( CPACK_DEBIAN_PACKAGE_RELEASE ${PRODUCT_VERSION_BUILD} )
    endif()
endif()

include( CPack )

if( NOT WIN32 )
    include( DebianPackage )

    set( DEBIAN_PACKAGE_BUILD_DEPENDS "libclang-9-dev | libclang-8-dev | libclang-7-dev | libclang-6-dev | libclang-dev (>=6)" )
    set( DEBIAN_PACKAGE_CMAKE_OPTIONS "-DENABLE_TEST=OFF" )
    set( DEBIAN_PACKAGE_COPYRIGHT_FILE "${CMAKE_SOURCE_DIR}/debian/copyright" )
    set( DEBIAN_PACKAGE_DISTRIBUTIONS "bionic" )

    add_debian_package_targets()
    add_dependencies( debian_package_binary build )
endif()

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
install( DIRECTORY include/ DESTINATION include )
