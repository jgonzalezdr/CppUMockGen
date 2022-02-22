if( NOT MSVC )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIT_TEST" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST" )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

find_package( LibClang REQUIRED )

find_package( CppUTest REQUIRED )

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc" )
endif()

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL ${PROD_SRC_FILES} ${TEST_SRC_FILES} ${CMAKE_CURRENT_LIST_DIR}/TestMain.cpp )

target_compile_definitions( ${PROJECT_NAME} PUBLIC _UNITTEST_ )

get_target_property( CPPUTEST_INCLUDE_DIR CppUTest INTERFACE_INCLUDE_DIRECTORIES )
target_compile_definitions( ${PROJECT_NAME} PUBLIC CPPUTEST_INCLUDE_DIR="${CPPUTEST_INCLUDE_DIR}" )

target_compile_definitions( ${PROJECT_NAME} PUBLIC CPPUMOCKGEN_INCLUDE_DIR="${PROD_SOURCE_DIR}/include" )

target_link_libraries( ${PROJECT_NAME} ${CppUTest_LIBRARIES} )

target_link_libraries( ${PROJECT_NAME} ${LibClang_LIBRARIES} )

if( (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0") )
    target_link_libraries( ${PROJECT_NAME} stdc++fs )
endif()

#
# C++ standard
#

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 1 )

add_dependencies( build_tests ${PROJECT_NAME} )

if( CI_MODE )
    set( TEST_ARGS -ojunit -v )
else()
    set( TEST_ARGS -v )
endif()

add_test( ${PROJECT_NAME} ${PROJECT_NAME} ${TEST_ARGS} )
