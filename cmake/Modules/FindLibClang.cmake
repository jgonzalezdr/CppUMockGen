if( WIN32 )
    set( LIBCLANG_PATHS "C:/Program Files/LLVM" )
    set( LIBCLANG_LIBNAME "libclang" )
else()
    set( LIBCLANG_PATHS "/usr/lib/llvm-10" "/usr/lib/llvm-9" "/usr/lib/llvm-8" "/usr/lib/llvm-7" "/usr/lib/llvm-6.0" "/usr/lib/llvm-5.0" "/usr/lib/llvm" )
    set( LIBCLANG_LIBNAME "clang" )
endif()

set( LIBCLANG_HINTS "" )
if( DEFINED ENV{LIBCLANG_HOME} )
    set( LIBCLANG_HINTS "$ENV{LIBCLANG_HOME}" ${LIBCLANG_HINTS} )
endif()
if( LIBCLANG_HOME )
    set( LIBCLANG_HINTS "${LIBCLANG_HOME}" ${LIBCLANG_HINTS} )
endif()
if( LibClang_HOME )
    set( LIBCLANG_HINTS "${LibClang_HOME}" ${LIBCLANG_HINTS} )
endif()

find_path( LibClang_BASE_DIR include/clang-c/Index.h PATHS ${LIBCLANG_PATHS} HINTS ${LIBCLANG_HINTS} )
mark_as_advanced( LibClang_BASE_DIR )

if( NOT WIN32 )
    string( REGEX MATCH "-[^-]+$" LIBCLANG_VERSION_SUFFIX ${LibClang_BASE_DIR} )
    if( LIBCLANG_VERSION_SUFFIX )
        set( LIBCLANG_LIBNAME "${LIBCLANG_LIBNAME}${LIBCLANG_VERSION_SUFFIX}" )
    endif()
endif()

find_library( LibClang_LIB_PATH NAMES ${LIBCLANG_LIBNAME} PATHS ${LibClang_BASE_DIR} ${LibClang_BASE_DIR}/lib )
mark_as_advanced( LibClang_LIB_PATH )

if( WIN32 )
    find_file( LibClang_DLL_PATH ${LIBCLANG_LIBNAME}.dll PATHS ${LibClang_BASE_DIR}/bin )
endif()

if( LibClang_DLL_PATH )
    add_library( LibClang SHARED IMPORTED )
    set_target_properties( LibClang PROPERTIES IMPORTED_IMPLIB ${LibClang_LIB_PATH} )
    set_target_properties( LibClang PROPERTIES IMPORTED_LOCATION ${LibClang_DLL_PATH} )
else()
    add_library( LibClang UNKNOWN IMPORTED )
    set_target_properties( LibClang PROPERTIES IMPORTED_LOCATION ${LibClang_LIB_PATH} )
endif()

#
# Export stuff
#

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( LibClang DEFAULT_MSG LibClang_BASE_DIR LibClang_LIB_PATH )

unset( LIBCLANG_PATHS )
unset( LIBCLANG_LIBNAME )
unset( LIBCLANG_VERSION_SUFFIX )

target_include_directories( LibClang INTERFACE ${LibClang_BASE_DIR}/include )

set( LibClang_LIBRARIES LibClang )
