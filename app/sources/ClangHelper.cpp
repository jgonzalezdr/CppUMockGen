#include "ClangHelper.hpp"

std::ostream& operator<<( std::ostream& stream, const CXString& cxstr )
{
    stream << clang_getCString(cxstr);
    clang_disposeString(cxstr);
    return stream;
}

std::string toString( const CXString & cxstr )
{
    std::string ret;
    ret += clang_getCString(cxstr);
    clang_disposeString(cxstr);
    return ret;
}

std::string& operator+=( std::string &string, const CXString &cxstr )
{
    string += toString( cxstr );
    return string;
}

// LCOV_EXCL_START
std::string operator+( const std::string &string, const CXString &cxstr )
{
    std::string ret = string;
    ret += toString( cxstr );
    return ret;
}
// LCOV_EXCL_STOP

std::string operator+( const char *string, const CXString &cxstr )
{
    std::string ret = string;
    ret += toString( cxstr );
    return ret;
}

std::string getQualifiedName( const CXCursor &cursor )
{
    std::string ret;

    CXCursorKind cursorKind = clang_getCursorKind( cursor );
    if( cursorKind != CXCursor_TranslationUnit )
    {
        CXCursor parent = clang_getCursorSemanticParent( cursor );

        if( !clang_Cursor_isNull( parent ) )
        {
            ret = getQualifiedName( parent );
        }

        if( !ret.empty() )
        {
            ret += "::";
        }

        ret += clang_getCursorSpelling( cursor );
    }

    return ret;
}

std::string getBareTypeSpelling( const CXType &type )
{
    std::string fullName = toString( clang_getTypeSpelling( (CXType &)type ) );

    size_t sepIdx = fullName.find_last_of( ' ' );

    if( sepIdx != std::string::npos )
    {
        // Assumption that libclang does not return type spellings with a space at the end
        char firstCharOfLastWord = fullName[sepIdx + 1];
        if( ( firstCharOfLastWord == '*' ) || ( firstCharOfLastWord == '&' ) )
        {
            fullName = fullName.substr( 0, sepIdx );
            sepIdx = fullName.find_last_of( ' ' );
        }
    }

    std::string className;
    if( sepIdx != std::string::npos )
    {
        className = fullName.substr( sepIdx + 1 );
    }
    else
    {
        className = fullName;
    }

    return className;
}

std::string getMethodClassName( const CXCursor &cursor )
{
    std::string ret;

    CXCursor parent = clang_getCursorSemanticParent( cursor );

    if( !clang_Cursor_isNull( parent ) )
    {
        ret = getQualifiedName( parent );
    }

    return ret;
}
