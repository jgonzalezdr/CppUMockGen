if( WIN32 )
    set( LCOV_PATHS "C:/lcov/bin" "C:/Program Files/lcov/bin" )
else()
    set( LCOV_PATHS "/usr/local/bin" "/usr/bin" )
endif()

find_program( lcov_EXECUTABLE lcov PATHS ${lcov_HOME}/bin $ENV{lcov_HOME}/bin ${LCOV_PATHS} )

get_filename_component( LCOV_BASE_DIR ${lcov_EXECUTABLE} DIRECTORY )

find_program( genhtml_EXECUTABLE genhtml PATHS ${LCOV_BASE_DIR} NO_DEFAULT_PATH )

if( lcov_EXECUTABLE )
    find_package( Perl REQUIRED )

    if( DEFINED Perl_FOUND )
        # Get version
        execute_process(
            COMMAND ${PERL_EXECUTABLE} ${lcov_EXECUTABLE} -v
            OUTPUT_VARIABLE LCOV_VERSION_OUTPUT_VARIABLE
            RESULT_VARIABLE LCOV_VERSION_RESULT_VARIABLE
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE )
            
        if( NOT LCOV_VERSION_RESULT_VARIABLE )
            string( REGEX REPLACE "^[^:]*: LCOV version (.+)$" "\\1" LCOV_VERSION_STRING ${LCOV_VERSION_OUTPUT_VARIABLE} )
        endif()
    endif()
endif()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( lcov 
                                   REQUIRED_VARS lcov_EXECUTABLE genhtml_EXECUTABLE
                                   VERSION_VAR LCOV_VERSION_STRING )

mark_as_advanced( lcov_EXECUTABLE genhtml_EXECUTABLE )

unset( LCOV_PATHS )
unset( LCOV_BASE_DIR )
unset( LCOV_VERSION_OUTPUT_VARIABLE )
unset( LCOV_VERSION_RESULT_VARIABLE )
