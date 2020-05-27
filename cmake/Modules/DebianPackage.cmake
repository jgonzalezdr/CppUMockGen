set( DEBIAN_PACKAGE_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR}/DebianPackage )

function( add_debian_package_targets )

    #
    # Parse input variables
    #

    if( NOT DEBIAN_PACKAGE_NAME )
        if( CPACK_DEBIAN_PACKAGE_NAME )
            set( DEBIAN_PACKAGE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}" )
        elseif( CPACK_PACKAGE_NAME )
            set( DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}" )
        elseif( PRODUCT_NAME )
            set( DEBIAN_PACKAGE_NAME "${PRODUCT_NAME}" )
        elseif( PROJECT_NAME )
            set( DEBIAN_PACKAGE_NAME "${PROJECT_NAME}" )
        else()
            message( FATAL_ERROR "Debian package name not defined" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_SECTION )
        if( CPACK_DEBIAN_PACKAGE_SECTION )
            set( DEBIAN_PACKAGE_SECTION "${CPACK_DEBIAN_PACKAGE_SECTION}" )
        else()
            set( DEBIAN_PACKAGE_SECTION "devel" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_PRIORITY )
        if( CPACK_DEBIAN_PACKAGE_PRIORITY )
            set( DEBIAN_PACKAGE_PRIORITY "${CPACK_DEBIAN_PACKAGE_PRIORITY}" )
        else()
            set( DEBIAN_PACKAGE_PRIORITY "optional" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_VERSION )
        if( CPACK_DEBIAN_PACKAGE_VERSION )
            set( DEBIAN_PACKAGE_VERSION "${CPACK_DEBIAN_PACKAGE_VERSION}" )
        elseif( CPACK_PACKAGE_VERSION )
            set( DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
        elseif( PRODUCT_VERSION )
            set( DEBIAN_PACKAGE_VERSION "${PRODUCT_VERSION}" )
        elseif( PROJECT_VERSION )
            set( DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}" )
        else()
            message( FATAL_ERROR "Debian package version not defined" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_RELEASE )
        if( CPACK_DEBIAN_PACKAGE_RELEASE )
            set( DEBIAN_PACKAGE_RELEASE "${CPACK_DEBIAN_PACKAGE_RELEASE}" )
        else()
            set( DEBIAN_PACKAGE_RELEASE 1 )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_DISTRIBUTIONS )
        set( DEBIAN_PACKAGE_DISTRIBUTIONS "UNRELEASED" )
    endif()

    if( NOT DEBIAN_PACKAGE_URGENCY )
        set( DEBIAN_PACKAGE_URGENCY "low" )
    endif()

    if( NOT DEBIAN_PACKAGE_MAINTAINER )
        if( CPACK_DEBIAN_PACKAGE_MAINTAINER )
            set( DEBIAN_PACKAGE_MAINTAINER "${CPACK_DEBIAN_PACKAGE_MAINTAINER}" )
        elseif( CPACK_PACKAGE_CONTACT )
            set( DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}" )
        else()
            message( FATAL_ERROR "Debian package maintainer not defined" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_HOMEPAGE )
        if( CPACK_DEBIAN_PACKAGE_HOMEPAGE )
            set( DEBIAN_PACKAGE_HOMEPAGE "${CPACK_DEBIAN_PACKAGE_HOMEPAGE}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_DESCRIPTION_FILE AND NOT DEBIAN_PACKAGE_DESCRIPTION )
        if( CPACK_DEBIAN_PACKAGE_DESCRIPTION )
            set( DEBIAN_PACKAGE_DESCRIPTION "${CPACK_DEBIAN_PACKAGE_DESCRIPTION}" )
        elseif( CPACK_PACKAGE_DESCRIPTION )
            set( DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}" )
        elseif( CPACK_PACKAGE_DESCRIPTION_FILE )
            set( DEBIAN_PACKAGE_DESCRIPTION_FILE ${CPACK_PACKAGE_DESCRIPTION_FILE} )
        elseif( CPACK_PACKAGE_DESCRIPTION_SUMMARY )
            set( DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}" )
        else()
            set( DEBIAN_PACKAGE_DESCRIPTION "${DEBIAN_PACKAGE_NAME}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_CHANGELOG )
        set( DEBIAN_PACKAGE_CHANGELOG "  * Release v${DEBIAN_PACKAGE_VERSION}" )
    endif()

    if( NOT DEBIAN_PACKAGE_ARCHITECTURE )
        if( CPACK_DEBIAN_PACKAGE_ARCHITECTURE )
            set( DEBIAN_PACKAGE_ARCHITECTURE "${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}" )
        else()
            execute_process(
                COMMAND dpkg --print-architecture
                OUTPUT_VARIABLE DEBIAN_PACKAGE_ARCHITECTURE
                RESULT_VARIABLE DEBIAN_PACKAGE_ARCHITECTURE_RES
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE )
            if( NOT DEBIAN_PACKAGE_ARCHITECTURE_RES )
                set( DEBIAN_PACKAGE_ARCHITECTURE "amd64" )
            endif()
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_DEPENDS )
        if( CPACK_DEBIAN_PACKAGE_DEPENDS )
            set( DEBIAN_PACKAGE_BINARY_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
        else()
            set( DEBIAN_PACKAGE_BINARY_DEPENDS "\\\${misc:Depends}, \\\${shlibs:Depends}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_PREDEPENDS )
        if( CPACK_DEBIAN_PACKAGE_PREDEPENDS )
            set( DEBIAN_PACKAGE_BINARY_PREDEPENDS "${CPACK_DEBIAN_PACKAGE_PREDEPENDS}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_RECOMMENDS )
        if( CPACK_DEBIAN_PACKAGE_RECOMMENDS )
            set( DEBIAN_PACKAGE_BINARY_RECOMMENDS "${CPACK_DEBIAN_PACKAGE_RECOMMENDS}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_SUGGESTS )
        if( CPACK_DEBIAN_PACKAGE_SUGGESTS )
            set( DEBIAN_PACKAGE_BINARY_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_BREAKS )
        if( CPACK_DEBIAN_PACKAGE_BREAKS )
            set( DEBIAN_PACKAGE_BINARY_BREAKS "${CPACK_DEBIAN_PACKAGE_BREAKS}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_CONFLICTS )
        if( CPACK_DEBIAN_PACKAGE_CONFLICTS )
            set( DEBIAN_PACKAGE_BINARY_CONFLICTS "${CPACK_DEBIAN_PACKAGE_CONFLICTS}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_ENHANCES )
        if( CPACK_DEBIAN_PACKAGE_ENHANCES )
            set( DEBIAN_PACKAGE_BINARY_ENHANCES "${CPACK_DEBIAN_PACKAGE_ENHANCES}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_REPLACES )
        if( CPACK_DEBIAN_PACKAGE_REPLACES )
            set( DEBIAN_PACKAGE_BINARY_REPLACES "${CPACK_DEBIAN_PACKAGE_REPLACES}" )
        endif()
    endif()

    if( NOT DEBIAN_PACKAGE_BINARY_PROVIDES )
        if( CPACK_DEBIAN_PACKAGE_PROVIDES )
            set( DEBIAN_PACKAGE_BINARY_PROVIDES "${CPACK_DEBIAN_PACKAGE_PROVIDES}" )
        endif()
    endif()

    #
    # Generate output variables
    #

    set( DEBIAN_PACKAGE_CONTROL_SOURCE_EXTRA_LINES "" )

    if( DEBIAN_PACKAGE_HOMEPAGE )
        string( APPEND DEBIAN_PACKAGE_CONTROL_SOURCE_EXTRA_LINES "Homepage: ${DEBIAN_PACKAGE_HOMEPAGE}\n" )
    endif()

    set( DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "" )

    if( DEBIAN_PACKAGE_BINARY_PREDEPENDS )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Pre-Depends: ${DEBIAN_PACKAGE_BINARY_PREDEPENDS}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_RECOMMENDS )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Recommends: ${DEBIAN_PACKAGE_BINARY_RECOMMENDS}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_SUGGESTS )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Suggests: ${DEBIAN_PACKAGE_BINARY_SUGGESTS}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_BREAKS )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Breaks: ${DEBIAN_PACKAGE_BINARY_BREAKS}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_CONFLICTS )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Conflicts: ${DEBIAN_PACKAGE_BINARY_CONFLICTS}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_ENHANCES )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Enhances: ${DEBIAN_PACKAGE_BINARY_ENHANCES}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_REPLACES )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Replaces: ${DEBIAN_PACKAGE_BINARY_REPLACES}\n" )
    endif()

    if( DEBIAN_PACKAGE_BINARY_PROVIDES )
        string( APPEND DEBIAN_PACKAGE_CONTROL_BINARY_EXTRA_LINES "Provides: ${DEBIAN_PACKAGE_BINARY_PROVIDES}\n" )
    endif()

    if( DEBIAN_PACKAGE_BUILD_DEPENDS )
        string( PREPEND DEBIAN_PACKAGE_BUILD_DEPENDS "cmake, debhelper (>=9), " )
    else()
        set( DEBIAN_PACKAGE_BUILD_DEPENDS "cmake, debhelper (>=9)" )
    endif()

    if( NOT DEBIAN_PACKAGE_COPYRIGHT_FILE )
        set( DEBIAN_PACKAGE_COPYRIGHT_HEADER_LINES "" )
        if( DEBIAN_PACKAGE_COPYRIGHT_UPSTREAM_NAME )
            string( APPEND DEBIAN_PACKAGE_COPYRIGHT_HEADER_LINES "Upstream-Name: ${DEBIAN_PACKAGE_COPYRIGHT_UPSTREAM_NAME}\n" )
        endif()
        if( DEBIAN_PACKAGE_COPYRIGHT_UPSTREAM_CONTACT )
            string( APPEND DEBIAN_PACKAGE_COPYRIGHT_HEADER_LINES "Upstream-Contact: ${DEBIAN_PACKAGE_COPYRIGHT_UPSTREAM_CONTACT}\n" )
        endif()
        if( DEBIAN_PACKAGE_COPYRIGHT_SOURCE )
            string( APPEND DEBIAN_PACKAGE_COPYRIGHT_HEADER_LINES "Source: ${DEBIAN_PACKAGE_COPYRIGHT_SOURCE}\n" )
        endif()
        if( DEBIAN_PACKAGE_COPYRIGHT_DISCLAIMER )
            string( APPEND DEBIAN_PACKAGE_COPYRIGHT_HEADER_LINES "Disclaimer: ${DEBIAN_PACKAGE_COPYRIGHT_DISCLAIMER}\n" )
        endif()
        if( NOT DEBIAN_PACKAGE_COPYRIGHT_HOLDER )
            message( FATAL_ERROR "Debian package copyright holder not defined" )
        endif()
        if( NOT DEBIAN_PACKAGE_COPYRIGHT_YEARS )
            message( FATAL_ERROR "Debian package copyright years not defined" )
        endif()
        if( NOT DEBIAN_PACKAGE_COPYRIGHT_LICENSE )
            message( FATAL_ERROR "Debian package copyright license not defined" )
        endif()
    endif()

    set( DEBIAN_PACKAGE_RULES_CMAKE_OPTIONS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" )
    if( DEBIAN_PACKAGE_CMAKE_OPTIONS )
        string( APPEND DEBIAN_PACKAGE_RULES_CMAKE_OPTIONS " ${DEBIAN_PACKAGE_CMAKE_OPTIONS}" )
    endif()

    if( NOT DEBIAN_PACKAGE_SOURCE_FILENAME )
        set( DEBIAN_PACKAGE_SOURCE_FILENAME "${DEBIAN_PACKAGE_NAME}-${DEBIAN_PACKAGE_VERSION}-Source" )
    endif()

    string( TOLOWER ${DEBIAN_PACKAGE_NAME} DEBIAN_PACKAGE_LOWER_NAME )

    string( TIMESTAMP DEBIAN_PACKAGE_DATE "%a, %d %b %Y %H:%M:%S +0000" UTC )

    if( DEBIAN_PACKAGE_SIGN_KEY AND DEBIAN_PACKAGE_NO_SIGN )
        message( FATAL_ERROR "Cannot activate both DEBIAN_PACKAGE_SIGN_KEY and DEBIAN_PACKAGE_NO_SIGN" )
    endif()

    #
    # Generate target
    #

    set( DEBIAN_PACKAGE_BUILD_DIR ${CMAKE_BINARY_DIR} )

    configure_file( ${DEBIAN_PACKAGE_TEMPLATE_DIR}/GenerateDebianPackage.in.cmake GenerateDebianPackage.cmake @ONLY )

    add_custom_target( debian_package_binary
                       COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package_source
                       COMMAND ${CMAKE_COMMAND} -DDEBIAN_PACKAGE_BINARY=ON -P ${CMAKE_CURRENT_BINARY_DIR}/GenerateDebianPackage.cmake )

    add_custom_target( debian_package_source
                       COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package_source
                       COMMAND ${CMAKE_COMMAND} -DDEBIAN_PACKAGE_BINARY=OFF -P ${CMAKE_CURRENT_BINARY_DIR}/GenerateDebianPackage.cmake )

endfunction()

