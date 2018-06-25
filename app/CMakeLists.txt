cmake_minimum_required( VERSION 3.3 )

project( CppUMockGen )
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 3 )
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
     sources/App.cpp
     sources/Parser.cpp
     sources/ClangHelper.cpp
     sources/Function.cpp
     sources/Method.cpp
     sources/ConsoleColorizer.cpp
     sources/Config.cpp
     sources/FileHelper.cpp
)

set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )

include( ${CMAKE_SOURCE_DIR}/cmake/generate_product_version.cmake )

generate_product_version(
    PRODUCT_VERSION_FILES
    NAME ${PROJECT_NAME}
    VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
    VERSION_MINOR ${PROJECT_VERSION_MINOR}
    VERSION_PATCH ${PROJECT_VERSION_PATCH}
    VERSION_BUILD ${PROJECT_VERSION_BUILD}
    VERSION_SUFFIX "${PROJECT_VERSION_SUFFIX}"
    COMPANY_NAME "www.gdr-sistemas.com"
    COMPANY_COPYRIGHT "(C) Copyright 2017-2018 Jesus Gonzalez"
    COMMENTS "Mock generator for CppUTest"
    EXPORT_TO_PARENT_SCOPE
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
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_COMMENTS} )
set( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt )

set( CPACK_NSIS_MODIFY_PATH OFF )
set( CPACK_NSIS_MUI_ICON "\\\${NSISDIR}\\\\Contrib\\\\Graphics\\\\Icons\\\\orange-install.ico" )
set( CPACK_NSIS_MUI_UNIICON "\\\${NSISDIR}\\\\Contrib\\\\Graphics\\\\Icons\\\\orange-uninstall.ico" )
string_escape( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr SHCTX \"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\${PRODUCT_NAME}.exe\" \"\" \"\$INSTDIR\\${PRODUCT_NAME}.exe\"" )
string_escape( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegKey SHCTX \"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\${PRODUCT_NAME}.exe\"" )

if( WIN32 )
    if( NOT INSTALLER_FILE_VERSION OR "${INSTALLER_FILE_VERSION}" STREQUAL "" )
        set( INSTALLER_FILE_VERSION "v${PRODUCT_VERSION}" )
    endif()

    set( CPACK_PACKAGE_FILE_NAME "Install ${PRODUCT_NAME} ${INSTALLER_FILE_VERSION}" )
endif()

include( CPack )

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
install( DIRECTORY include/ DESTINATION include ) 
