#include "ClangParseHelper.hpp"

void ClangParseHelper::ParseFunctions( const std::string &code, const std::function<void(CXCursor)> &callback )
{
    CXIndex index = clang_createIndex( 0, 1 );

    const char* clangOpts[] = { "-xc++" };

    CXUnsavedFile unsavedFiles[] = { { "test.hpp", code.c_str(), (unsigned long) code.length() }  };

    CXTranslationUnit tu = clang_parseTranslationUnit( index, "test.hpp",
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
