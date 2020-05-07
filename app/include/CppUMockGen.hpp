/**
 * @file
 * @brief      Generic header for including CppUMockGen support
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_HPP_
#define CPPUMOCKGEN_HPP_

#include <type_traits>

namespace CppUMockGen {

/**
 * Enum used to identify parameters that shall be ignored.
 */
enum class IgnoreParameter { YES };

/**
 * Class used to handle parameters that can be ignored of expectation helper functions.
 */
template<typename BaseType>
class Parameter {
public:
    /**
     * Constructor for non-ignored parameters.
     * @param value [in] Value of the parameter
     */
    Parameter(const BaseType &value) : m_value(&value), m_isIgnored(false) {}

    /**
     * Constructor for ignored parameters.
     *
     * The actual value of the passed parameter is ignored.
     */
    Parameter(IgnoreParameter) : m_value(nullptr),  m_isIgnored(true) {}

    /**
     * Returns the parameter value.
     * @return Parameter value
     */
    const BaseType& getValue()
    {
        return *m_value;
    }

    /**
     * Indicates if the parameter is ignored.
     * @return @c true if the parameter is ignored, @c false otherwise
     */
    bool isIgnored()
    {
        return m_isIgnored;
    }

private:
    typename std::remove_reference<const BaseType>::type *m_value;
    bool m_isIgnored;
};

}

#endif // header guard
