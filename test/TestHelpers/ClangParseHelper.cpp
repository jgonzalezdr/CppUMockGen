/**
 * @file
 * @brief      Implementation of Clang parsing helper class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "ClangParseHelper.hpp"

#include <stdexcept>

void ClangParseHelper::ParseHeader( const std::string &code, const std::function<void(CXCursor)> &callback )
{
    CXIndex index = clang_createIndex( 0, 1 );

#ifdef INTERPRET_C
    const char* clangOpts[] = { "-xc" };
#else
    const char* clangOpts[] = { "-xc++" };
#endif

    CXUnsavedFile unsavedFiles[] = { { "test.h", code.c_str(), (unsigned long) code.length() }  };

    CXTranslationUnit tu = clang_parseTranslationUnit( index, "test.h",
                                                       clangOpts, std::extent<decltype(clangOpts)>::value,
                                                       unsavedFiles, std::extent<decltype(unsavedFiles)>::value,
                                                       CXTranslationUnit_None );
    if( tu == nullptr )
    {
        throw std::runtime_error( "Error creating translation unit" );
    }

    CXCursor tuCursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren( tuCursor, []( CXCursor cursor, CXCursor parent, CXClientData client_data )
    {
        if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) != 0 )
        {
            CXCursorKind cursorKind = clang_getCursorKind( cursor );
            if( ( cursorKind == CXCursor_FunctionDecl ) || ( cursorKind == CXCursor_CXXMethod ) )
            {
                ( * ( std::function<void( CXCursor )>* ) client_data )( cursor );
                return CXChildVisit_Continue;
            }
            else
            {
                return CXChildVisit_Recurse;
            }
        }
        else
        {
            return CXChildVisit_Continue;
        }
    },
        (CXClientData) &callback );

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );
}
