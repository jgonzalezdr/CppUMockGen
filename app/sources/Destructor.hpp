/**
 * @file
 * @brief      Header of Destructor class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_DESTRUCTOR_HPP_
#define CPPUMOCKGEN_DESTRUCTOR_HPP_

#include "Function.hpp"

/**
 * Class used to parse destructors and generate mocks and expectations.
 */
class Destructor : public Function
{
public:
    /**
     * Default constructor.
     */
    Destructor() noexcept;

protected:
    virtual bool IsMockable( const CXCursor &cursor ) const noexcept override;
    virtual std::string GetExpectationFunctionName( const std::string &functionName ) const noexcept override;
};

#endif // header guard
