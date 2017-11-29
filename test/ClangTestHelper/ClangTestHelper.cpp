#include "ClangTestHelper.hpp"

ClangTestHelper::ClangTestHelper( const std::string &code )
    : m_code( code )
{
}

ClangTestHelper::~ClangTestHelper()
{
}

void ClangTestHelper::ParseFunctions( const std::function<void(CXCursor)> &callback )
{
    CXIndex m_index = clang_createIndex( 0, 1 );

    const char* clangOpts[] = { "-xc++" };

    CXUnsavedFile unsavedFiles[] = { { "test.hpp", m_code.c_str(), m_code.length() }  };

    CXTranslationUnit m_tu = clang_parseTranslationUnit( m_index, "test.hpp", 
                                                         clangOpts, std::extent<decltype(clangOpts)>::value, 
                                                         unsavedFiles, std::extent<decltype(unsavedFiles)>::value, 
                                                         CXTranslationUnit_None );
    if( m_tu == nullptr )
    {
        throw std::runtime_error( "Error creating translation unit" );
    }

    CXCursor tuCursor = clang_getTranslationUnitCursor(m_tu);
    clang_visitChildren(
        tuCursor,
        []( CXCursor cursor, CXCursor parent, CXClientData client_data )
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

    clang_disposeTranslationUnit( m_tu );
    clang_disposeIndex( m_index );
}
