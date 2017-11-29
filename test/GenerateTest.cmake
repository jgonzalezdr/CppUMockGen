if( NOT DEFINED CPPUTEST_HOME )
    set( CPPUTEST_HOME $ENV{CPPUTEST_HOME} )
endif()

if( NOT DEFINED CPPUTEST_HOME )
    message(FATAL_ERROR "CPPUTEST_HOME variable must define the path of the CppUTest home directory")
endif()

if( NOT MSVC )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --no-rtti" )
endif( NOT MSVC )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIT_TEST" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST" )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

find_package( LibClang REQUIRED )

include_directories( ${CPPUTEST_HOME}/include ${LibClang_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/test/ClangTestHelper )
link_directories( ${CPPUTEST_HOME}/lib ${LibClang_LIBRARY_DIRS} )

if( MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc" )
endif()

add_executable( ${PROJECT_NAME} ${PROD_SRC_FILES} ${TEST_SRC_FILES} ${CMAKE_CURRENT_LIST_DIR}/TestMain.cpp )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 1 )

if( MSVC )
    target_link_libraries( ${PROJECT_NAME} debug CppUTest64d ${LibClang_LIBRARIES} optimized CppUTest64 ${LibClang_LIBRARIES} )
else()
    target_link_libraries( ${PROJECT_NAME} CppUTest64 CppUTestExt64 ${LibClang_LIBRARIES} gcov pthread )
endif( MSVC )

add_dependencies( build ${PROJECT_NAME} )
add_dependencies( run_tests ${PROJECT_NAME} )

if( CI_MODE )
    set( TEST_ARGS -ojunit )
else( CI_MODE )
    set( TEST_ARGS -v )
endif( CI_MODE )

add_test( ${PROJECT_NAME} ${PROJECT_NAME} ${TEST_ARGS} )
