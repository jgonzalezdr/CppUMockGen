# Function to create Visual Studio source groups from directories 
#
# Copyright (c) 2014-2018 Jesus Gonzalez. All rights reserved.
#
# License: See LICENSE.txt

function( generate_groups BASE_DIR CURR_DIR )
    string( REPLACE "/" "\\" GROUP_NAME "${CURR_DIR}" )
    source_group( "Source Files\\${GROUP_NAME}" REGULAR_EXPRESSION ${BASE_DIR}/${CURR_DIR}.*\\.\(cpp\|c\|cxx\|cc\) )
    source_group( "Header Files\\${GROUP_NAME}" REGULAR_EXPRESSION ${BASE_DIR}/${CURR_DIR}.*\\.\(hpp\|h\|hxx\|hh\) )
    file( GLOB FILE_LIST RELATIVE ${BASE_DIR} ${BASE_DIR}/${CURR_DIR}/* )
    foreach( FILE ${FILE_LIST} )
        if( IS_DIRECTORY ${BASE_DIR}/${FILE} )
            generate_groups( ${BASE_DIR} ${FILE} )
        endif()
    endforeach()
endfunction()