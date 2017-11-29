#include "Method.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Method::Method( const CXCursor &cursor, const Config &config )
    : Function( cursor, config )
{}

bool Method::IsMockable() const
{
    // Only public methods should be mocked
    bool isPublic = ( clang_getCXXAccessSpecifier( m_cursor ) == CX_CXXPublic );

    // Pure virtual methods should not be mocked
    bool isPureVirtual = clang_CXXMethod_isPureVirtual( m_cursor );

    // TODO: Print warning if not mockable

    return Function::IsMockable() && isPublic && !isPureVirtual;
}

std::string Method::GenerateMock() const
{
    return Function::GenerateMock(true);
}
