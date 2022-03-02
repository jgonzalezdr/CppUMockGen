/**
 * @file
 * @brief      Header of Constructor class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_CONSTRUCTOR_HPP_
#define CPPUMOCKGEN_CONSTRUCTOR_HPP_

#include "Function.hpp"

/**
 * Class used to parse constructors and generate mocks and expectations.
 */
class Constructor : public Function
{
public:
    /**
     * Default constructor.
     */
    Constructor() noexcept;

protected:
    virtual bool IsMockable( const CXCursor &cursor ) const noexcept override;
    virtual std::string GetExpectationFunctionName( const std::string &functionName ) const noexcept override;
};

#endif // header guard
