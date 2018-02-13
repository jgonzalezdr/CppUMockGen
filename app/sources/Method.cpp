#include "Method.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Method::Method()
{}

bool Method::IsMockable( const CXCursor &cursor ) const
{
    // Only public methods should be mocked
    bool isPublic = ( clang_getCXXAccessSpecifier( cursor ) == CX_CXXPublic );

    // Pure virtual methods should not be mocked
    bool isVirtual = clang_CXXMethod_isVirtual( cursor );

    // Pure virtual methods should not be mocked
    bool isPureVirtual = clang_CXXMethod_isPureVirtual( cursor );

    // TODO: Print warning if not mockable

    return Function::IsMockable( cursor ) && (isPublic || isVirtual) && !isPureVirtual;
}
