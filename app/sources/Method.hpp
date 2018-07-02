/**
 * @file
 * @brief      Header of Method class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_METHOD_HPP_
#define CPPUMOCKGEN_METHOD_HPP_

#include "Function.hpp"

/**
 * Class used to parse methods (a.k.a member functions) and generate mocks and expectations.
 */
class Method : public Function
{
public:
    /**
     * Default constructor.
     */
    Method() noexcept;

protected:
    virtual bool IsMockable( const CXCursor &cursor ) const noexcept override;

    virtual bool IsMethod() const noexcept override
    {
        return true;
    }
};

#endif // header guard
