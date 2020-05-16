option( COVERAGE_VERBOSE "Coverage analysis is performed in verbose mode" OFF )

if( CI_MODE OR MSVC )
    set( IGNORE_ERROR || exit /b 0 )
endif()

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

# Test errors are ignored with MSVC to avoid the IDE nagging
add_custom_target( run_tests COMMAND ctest -V ${IGNORE_ERROR} DEPENDS build )

#
# xUnit report merging
#

if( CI_MODE AND NOT WIN32 )

    find_program( MERGE_XUNIT merge_xunit_results.py )
    if( NOT EXISTS ${MERGE_XUNIT} )
        message( FATAL_ERROR "merge_xunit_results.py is not installed" )
    endif()

    set( XUNIT_OUT_PATH "reports/xunit/" )
    set( XUNIT_OUT_FILE "xunit.xml" )

    add_custom_target( xunit_report
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${XUNIT_OUT_PATH}
                       COMMAND ${MERGE_XUNIT} -o ${XUNIT_OUT_PATH}${XUNIT_OUT_FILE} "cpputest_*.xml"
                       DEPENDS run_tests )

    add_dependencies( ci xunit_report )

endif()

#
# LCOV
#

if( COVERAGE AND NOT MSVC )

    set( CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_COVERAGE} -g -O0 -fprofile-arcs -ftest-coverage" )
    set( CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} -g -O0 -fprofile-arcs -ftest-coverage -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls" )
    set( CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_COVERAGE} -fprofile-arcs -ftest-coverage" )

    find_package( Perl REQUIRED )

    find_package( lcov REQUIRED )

    if( COVERAGE_OUTDIR )
        set( COVDST_DIR ${COVERAGE_OUTDIR} )
    else()
        if( CI_MODE )
            set( COVDST_DIR reports/coverage )
        else()
            set( COVDST_DIR ${CMAKE_SOURCE_DIR}/coverage_lcov )
        endif()
    endif()

    set( LCOV_ARGS --rc lcov_branch_coverage=exclude-exceptions )

    set( COVSRC_DIR ${CMAKE_SOURCE_DIR} )

    if( NOT COVERAGE_VERBOSE )
        set( LCOV_ARGS -q ${LCOV_ARGS} )
    endif()

    if( NOT COVERAGE_INCLUDED )
        set( COVERAGE_INCLUDED \"*/app/sources/*\" )
    endif()

    if( WIN32 )
        string( REPLACE / \\\\ COVERAGE_INCLUDED ${COVERAGE_INCLUDED} )
        if( DEFINED COVERAGE_EXCLUDED )
            string( REPLACE / \\\\ COVERAGE_EXCLUDED ${COVERAGE_EXCLUDED} )
        endif()
    endif()

    add_custom_target( coverage_clean
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${COVDST_DIR}
                       COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} -z -d ${CMAKE_BINARY_DIR}
                       DEPENDS build )

    add_custom_target( coverage_initial
                       COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -c -i -d ${CMAKE_BINARY_DIR} -b ${COVSRC_DIR} -o ${COVDST_DIR}/app_base.info
                       DEPENDS coverage_clean )
                       
    add_custom_target( run_coverage_tests
                       COMMAND ctest -V ${IGNORE_ERROR}
                       DEPENDS coverage_initial )

    add_custom_target( coverage_process_test_data
                       COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -c -d ${CMAKE_BINARY_DIR} -b ${COVSRC_DIR} -o ${COVDST_DIR}/app_test.info
                       DEPENDS run_coverage_tests )

    add_custom_target( coverage_combine
                       COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -a ${COVDST_DIR}/app_base.info -a ${COVDST_DIR}/app_test.info -o ${COVDST_DIR}/app_full.info
                       DEPENDS coverage_process_test_data )

    if( DEFINED COVERAGE_EXCLUDED )
        add_custom_target( coverage_filter_included
                           COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -e ${COVDST_DIR}/app_full.info -o ${COVDST_DIR}/app_stripped_i.info ${COVERAGE_INCLUDED}
                           DEPENDS coverage_combine )

        add_custom_target( coverage_process
                           COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -r ${COVDST_DIR}/app_stripped_i.info -o ${COVDST_DIR}/app_stripped.info ${COVERAGE_EXCLUDED}
                           DEPENDS coverage_filter_included )
    else()
        add_custom_target( coverage_process
                           COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} ${LCOV_ARGS} -e ${COVDST_DIR}/app_full.info -o ${COVDST_DIR}/app_stripped.info ${COVERAGE_INCLUDED}
                           DEPENDS coverage_combine )
    endif()

    if( CI_MODE AND JENKINS )

        find_program( LCOV_XML lcov_cobertura.py )
        if( NOT EXISTS ${LCOV_XML} )
            message( FATAL_ERROR "lcov_cobertura is not installed" )
        endif()

        set( COVERAGE_FILE coverage.xml )

        add_custom_target( coverage_report
                           COMMAND ${LCOV_XML} -b ${COVSRC_DIR} -o ${COVDST_DIR}/${COVERAGE_FILE} ${COVDST_DIR}/app_stripped.info
                           DEPENDS coverage_process )

        add_dependencies( ci coverage_report )

    elseif( NOT CI_MODE )

        add_custom_target( coverage_report
                           COMMAND ${PERL_EXECUTABLE} ${genhtml_EXECUTABLE} ${LCOV_ARGS} -s ${COVDST_DIR}/app_stripped.info -o ${COVDST_DIR} --demangle-cpp --title "Unit Tests" --rc genhtml_charset=cp-1252
                           DEPENDS coverage_process )

    endif()

endif( )

if( COVERAGE AND MSVC )

    find_program( OPENCPPCOVERAGE OpenCppCoverage )
    if( NOT EXISTS ${OPENCPPCOVERAGE} )
        message( FATAL_ERROR "OpenCppCoverage is not installed" )
    endif()

    if( COVERAGE_OUTDIR )
        set( COVDST_DIR ${COVERAGE_OUTDIR} )
    else()
        if( CI_MODE )
            set( COVDST_DIR reports/coverage )
        else()
            set( COVDST_DIR ${CMAKE_SOURCE_DIR}/coverage_msvc )
        endif()
    endif()

    set( OPENCPPCOV_ARGS --cover_children )
    set( COVSRC_PATH ${CMAKE_SOURCE_DIR}/app/sources/* )

    if( NOT COVERAGE_VERBOSE )
        set( OPENCPPCOV_ARGS -q ${OPENCPPCOV_ARGS} )
    endif()

    if( CI_MODE )
        set( COV_OUTTYPE cobertura )
    else()
        set( COV_OUTTYPE html )
    endif()

    string( REGEX REPLACE "/" "\\\\" COVSRC_PATH ${COVSRC_PATH} )
    string( REGEX REPLACE "/" "\\\\" COVDST_DIR ${COVDST_DIR} )

    add_custom_target( run_coverage
                       COMMAND ${OPENCPPCOVERAGE} ${OPENCPPCOV_ARGS} --export_type ${COV_OUTTYPE}:${COVDST_DIR} --sources ${COVSRC_PATH} -- ${CMAKE_CTEST_COMMAND} ${CTEST_ARGS}
                       DEPENDS build )

endif( )
