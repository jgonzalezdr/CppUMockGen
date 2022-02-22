set( CPPUTEST_LIBNAME "CppUTest" )
set( CPPUTESTEXT_LIBNAME "CppUTestExt" )

if( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
    set( CPPUTEST_LIBNAME "CppUTest64" ${CPPUTEST_LIBNAME} )
    set( CPPUTESTEXT_LIBNAME "CppUTestExt64" ${CPPUTESTEXT_LIBNAME} )
elseif( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )
    set( CPPUTEST_LIBNAME "CppUTest32" ${CPPUTEST_LIBNAME} )
    set( CPPUTESTEXT_LIBNAME "CppUTestExt32" ${CPPUTESTEXT_LIBNAME} )
endif()

if( WIN32 )
    set( CPPUTEST_PATHS "C:/Program Files/CppUTest" "C:/CppUTest" )
else()
    set( CPPUTEST_PATHS "/usr/local" "/usr" )
endif()

set( CPPUTEST_HINTS "" )
if( DEFINED ENV{CPPUTEST_HOME} )
    set( CPPUTEST_HINTS "$ENV{CPPUTEST_HOME}/lib" ${CPPUTEST_HINTS} )
endif()
if( CPPUTEST_HOME )
    set( CPPUTEST_HINTS "${CPPUTEST_HOME}/lib" ${CPPUTEST_HINTS} )
endif()
if( CppUTest_HOME )
    set( CPPUTEST_HINTS "${CppUTest_HOME}/lib" ${CPPUTEST_HINTS} )
endif()

#
# Find CppUTest library
#

find_library( CppUTest_LIB_PATH NAMES ${CPPUTEST_LIBNAME} PATHS ${CPPUTEST_PATHS} HINTS ${CPPUTEST_HINTS} )
mark_as_advanced( CppUTest_LIB_PATH )

add_library( CppUTest UNKNOWN IMPORTED )
set_target_properties( CppUTest PROPERTIES IMPORTED_LOCATION ${CppUTest_LIB_PATH} )

#
# Find CppUTestExt library
#

get_filename_component( CppUTest_LIBRARY_DIR ${CppUTest_LIB_PATH} DIRECTORY )

find_library( CppUTestExt_LIB_PATH NAMES ${CPPUTESTEXT_LIBNAME} HINTS ${CppUTest_LIBRARY_DIR} NO_DEFAULT_PATH )
mark_as_advanced( CppUTestExt_LIB_PATH  )

if( CppUTestExt_LIB_PATH )
    add_library( CppUTestExt UNKNOWN IMPORTED )
    set_target_properties( CppUTestExt PROPERTIES IMPORTED_LOCATION ${CppUTestExt_LIB_PATH} )
endif()

#
# Find CppUTest debug library
#

list( TRANSFORM CPPUTEST_LIBNAME APPEND "d" OUTPUT_VARIABLE CPPUTEST_DBG_LIBNAME )
find_library( CppUTest_DEBUG_LIB_PATH NAMES ${CPPUTEST_DBG_LIBNAME} HINTS ${CppUTest_LIBRARY_DIR} NO_DEFAULT_PATH )
mark_as_advanced( CppUTest_DEBUG_LIB_PATH  )

if( CppUTest_DEBUG_LIB_PATH )
    set_target_properties( CppUTest PROPERTIES IMPORTED_LOCATION_DEBUG ${CppUTest_DEBUG_LIB_PATH} )
endif()

#
# Find CppUTestExt debug library
#

list( TRANSFORM CPPUTESTEXT_LIBNAME APPEND "d" OUTPUT_VARIABLE CPPUTESTEXT_DBG_LIBNAME )
find_library( CppUTestExt_DEBUG_LIB_PATH NAMES ${CPPUTESTEXT_DBG_LIBNAME} HINTS ${CppUTest_LIBRARY_DIR} NO_DEFAULT_PATH )
mark_as_advanced( CppUTestExt_DEBUG_LIB_PATH  )

if( CppUTestExt_DEBUG_LIB_PATH )
    set_target_properties( CppUTestExt PROPERTIES IMPORTED_LOCATION_DEBUG ${CppUTestExt_DEBUG_LIB_PATH} )
endif()

#
# Export stuff
#

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CppUTest DEFAULT_MSG CppUTest_LIB_PATH CppUTestExt_LIB_PATH )

unset( CPPUTEST_PATHS )
unset( CPPUTEST_LIBNAME )

get_filename_component( CppUTest_ROOT ${CppUTest_LIBRARY_DIR} DIRECTORY )
target_include_directories( CppUTest INTERFACE ${CppUTest_ROOT}/include )

set( CppUTest_LIBRARIES CppUTest CppUTestExt )

if( WIN32 )
    target_link_libraries( CppUTest INTERFACE winmm.lib )
endif()

