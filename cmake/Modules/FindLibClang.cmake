if( WIN32 )
    set( LIBCLANG_PATHS "C:/Program Files/LLVM" )
    set( LIBCLANG_LIBNAME "libclang" )
else()
    set( LIBCLANG_LIBNAME "clang" )
endif()

find_path( LibClang_BASE_DIR include/clang-c/Index.h PATHS ${LIBCLANG_PATHS} )

find_library( LibClang_LIB_PATH NAMES ${LIBCLANG_LIBNAME} PATHS ${LibClang_BASE_DIR} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( LibClang DEFAULT_MSG LibClang_BASE_DIR LibClang_LIB_PATH )

mark_as_advanced( LibClang_BASE_DIR LibClang_LIB_PATH )

unset( LIBCLANG_PATHS )
unset( LIBCLANG_LIBNAME )

set( LibClang_INCLUDE_DIRS ${LibClang_BASE_DIR}/include )
set( LibClang_LIBRARIES ${LibClang_LIB_PATH} )
get_filename_component( LibClang_LIBRARY_DIRS ${LibClang_LIB_PATH} DIRECTORY )
