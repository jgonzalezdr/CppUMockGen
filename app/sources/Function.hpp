#ifndef FUNCTION_HPP_
#define FUNCTION_HPP_

#include <clang-c/Index.h>
#include <string>
#include "Config.hpp"

/**
 * Mock generator for functions.
 */
class Function
{
public:
    /**
     * Constructs a Function object.
     *
     * @param cursor [in] Cursor representing a function
     * @param config [in] Configuration to be used for mock generation
     */
    Function( const CXCursor &cursor, const Config &config );

    virtual ~Function();

    /**
     * Returns whether the function (or method) can be mocked.
     */
    virtual bool IsMockable() const;

    /**
     * Generates a mock for the function (or method).
     *
     * @return String containing the generated mock
     */
    virtual std::string GenerateMock() const;

protected:
    const CXCursor &m_cursor;
    const Config &m_config;

    std::string GenerateMock( bool isMethod ) const;
};

#endif // header guard
