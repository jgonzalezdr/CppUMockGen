/**
 * @file
 * @brief      Implementation of Constructor class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "Constructor.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Constructor::Constructor() noexcept
{}

bool Constructor::IsMockable( const CXCursor &cursor ) const noexcept
{
    // Public and protected constructors should be mocked
    bool isNotPrivate = isCursorNonPrivate( cursor );

    // TODO: Print warning if not mockable

    return Function::IsMockable( cursor ) && isNotPrivate;
}

std::string Constructor::GetExpectationFunctionName( const std::string &functionName ) const noexcept
{
    return functionName + "$ctor";
}
