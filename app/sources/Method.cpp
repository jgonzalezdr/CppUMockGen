/**
 * @file
 * @brief      Implementation of Method class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "Method.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Method::Method() noexcept
{}

bool Method::IsMockable( const CXCursor &cursor ) const noexcept
{
    // Public methods should be mocked
    bool isPublic = isCursorPublic( cursor );

    // Virtual methods should be mocked
    bool isVirtual = clang_CXXMethod_isVirtual( cursor );

    // Pure virtual methods should never be mocked
    bool isPureVirtual = clang_CXXMethod_isPureVirtual( cursor );

    // TODO: Print warning if not mockable

    return Function::IsMockable( cursor ) && (isPublic || isVirtual) && !isPureVirtual;
}
