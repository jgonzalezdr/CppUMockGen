/**
 * @file
 * @brief      Header of Function class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_FUNCTION_HPP_
#define CPPUMOCKGEN_FUNCTION_HPP_

#include <clang-c/Index.h>
#include <string>
#include <vector>
#include <memory>
#include "Config.hpp"

/**
 * Class used to parse functions and generate mocks and expectations.
 */
class Function
{
public:
    /**
     * Default constructor.
     */
    Function() noexcept;

    /**
     * Default destructor.
     */
    virtual ~Function() noexcept;

    /**
     * Parses a function.
     *
     * @param[in] cursor Cursor representing a function
     * @param[in] config Configuration to be used for parsing
     */
    bool Parse( const CXCursor &cursor, const Config &config );

    /**
     * Generates a mock for the function.
     *
     * @return String containing the generated mock
     */
    std::string GenerateMock() const noexcept;

    /**
     * Generates an expectation function prototype or implementation for the function.
     *
     * @param[in] proto Generate function prototype if @c true, or function implementation otherwise.
     * @return String containing the generated expectation function prototype
     */
    std::string GenerateExpectation( bool proto ) const noexcept;

    class Argument;
    class Return;

    enum class EExceptionSpec
    {
        Any,
        None,
        DynamicNone,
        Dynamic,
        MSAny
    };

protected:
    /**
     * Returns whether the function can be mocked.
     */
    virtual bool IsMockable( const CXCursor &cursor ) const noexcept;

    virtual std::string GetExpectationFunctionName( const std::string &functionName ) const noexcept;

    std::string GenerateExpectation( bool proto, const std::string &functionName, bool oneCall ) const noexcept;

    bool HasIgnorableArguments() const noexcept;
    bool HasSkippedArguments() const noexcept;

    std::string m_functionName;
    std::unique_ptr<Return> m_return;
    std::vector<std::unique_ptr<Argument>> m_arguments;
    bool m_isConstMethod;
    bool m_isNonStaticMethod;
    EExceptionSpec m_exceptionSpec;
    std::string m_className;
};

#endif // header guard
