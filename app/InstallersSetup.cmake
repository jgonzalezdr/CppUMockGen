#
# Installers setup
#

option( ENABLE_INSTALLER "Enable building installers" ON )

if( ENABLE_INSTALLER )
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
        set( CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION} )
        set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
        set( CPACK_DEBIAN_PACKAGE_RECOMMENDS "cpputest" )
        if( NOT PRODUCT_PRERELEASE )
            set( CPACK_DEBIAN_PACKAGE_RELEASE ${PRODUCT_VERSION_BUILD} )
        endif()
    endif()

    include( CPack )

    if( NOT WIN32 )
        include( DebianPackage )

        set( DEBIAN_PACKAGE_BUILD_DEPENDS "g++-8, libclang-9-dev | libclang-8-dev | libclang-7-dev | libclang-6-dev | libclang-dev (>=6)" )

        set( DEBIAN_PACKAGE_CMAKE_OPTIONS "-DENABLE_TEST=OFF -DCMAKE_C_COMPILER=gcc-8 -DCMAKE_CXX_COMPILER=g++-8" )
        set( DEBIAN_PACKAGE_COPYRIGHT_FILE "${CMAKE_SOURCE_DIR}/debian/copyright" )
        set( DEBIAN_PACKAGE_DISTRIBUTIONS "bionic" )

        add_debian_package_targets()
        add_dependencies( debian_package_binary build )
    endif()
endif()
