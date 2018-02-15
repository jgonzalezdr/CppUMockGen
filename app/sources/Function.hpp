#ifndef FUNCTION_HPP_
#define FUNCTION_HPP_

#include <clang-c/Index.h>
#include <string>
#include <vector>
#include <memory>
#include "Config.hpp"

/**
 * Class used to parse functions and generate mocks.
 */
class Function
{
public:
    /**
     * Default constructor.
     */
    Function();

    /**
     * Default destructor.
     */
    virtual ~Function();

    /**
     * Parses a function.
     *
     * @param cursor [in] Cursor representing a function
     * @param config [in] Configuration to be used for parsing
     */
    bool Parse( const CXCursor &cursor, const Config &config );

    /**
     * Generates a mock for the function.
     *
     * @return String containing the generated mock
     */
    std::string GenerateMock() const;

    class Argument;
    class Return;

protected:
    /**
     * Returns whether the function can be mocked.
     */
    virtual bool IsMockable( const CXCursor &cursor ) const;

    /**
     * Returns whether the object is a method (a.k.a member function).
     */
    virtual bool IsMethod() const
    {
        return false;
    }

    std::string m_functionName;
    std::unique_ptr<Return> m_return;
    std::vector<std::unique_ptr<Argument>> m_arguments;
    bool m_isConst;
};

#endif // header guard
