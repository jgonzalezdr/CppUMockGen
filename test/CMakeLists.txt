cmake_minimum_required( VERSION 3.3 )

option( CHECK_COMPILATION "Check that generated code compiles properly" ON )

if( BUILD_TESTING )

    include( CommonTestTargets.cmake )

    set( TEST_BASE_DIR ${CMAKE_CURRENT_LIST_DIR} )
    set( PROD_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../app )
    set( PROD_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/../app )
    set( EXPECTATIONS_DIR ${CMAKE_CURRENT_LIST_DIR}/Expectations )
    set( MOCKS_DIR ${CMAKE_CURRENT_LIST_DIR}/Mocks )
    set( HELPERS_DIR ${CMAKE_CURRENT_LIST_DIR}/TestHelpers )

    include_directories(
        ${PROD_SOURCE_DIR}/sources
        ${PROD_SOURCE_DIR}/include/
        ${MOCKS_DIR}
        ${EXPECTATIONS_DIR}
        ${HELPERS_DIR}
    )

    if( NOT CHECK_COMPILATION )
        add_definitions( -DDISABLE_COMPILATION_CHECK )
    endif()

    # Test modules

    add_subdirectory( App )
    add_subdirectory( ConfigFile )
    add_subdirectory( Parser )
    add_subdirectory( Config )
    add_subdirectory( Function_Mock_CPP )
    add_subdirectory( Function_Mock_C )
    add_subdirectory( Function_Expectation_CPP )
    add_subdirectory( Function_Expectation_C )
    add_subdirectory( Method_Mock )
    add_subdirectory( Method_Expectation )
    add_subdirectory( Constructor_Mock )
    add_subdirectory( Constructor_Expectation )
    add_subdirectory( Destructor_Mock )
    add_subdirectory( Destructor_Expectation )
    add_subdirectory( OutputFileParser )

endif()
