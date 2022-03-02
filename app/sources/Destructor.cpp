/**
 * @file
 * @brief      Implementation of Destructor class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "Destructor.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Destructor::Destructor() noexcept
{}

bool Destructor::IsMockable( const CXCursor &cursor ) const noexcept
{
    // Only destructors in public classes should be mocked
    bool isClassPublic = isMemberInPublicClass( cursor );

    // TODO: Print warning if not mockable

    return Function::IsMockable( cursor ) && isClassPublic;
}

std::string Destructor::GetExpectationFunctionName( const std::string &functionName ) const noexcept
{
    return functionName.substr( 1 ) + "$dtor";
}
