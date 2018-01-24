#ifndef METHOD_HPP_
#define METHOD_HPP_

#include "Function.hpp"

/**
 * Mock generator for methods.
 */
class Method : public Function
{
public:
    /**
     * Constructs a Method object.
     *
     * @param cursor [in] Cursor representing a method
     * @param config [in] Configuration to be used for mock generation
     */
    Method( const CXCursor &cursor, const Config &config );

    virtual bool IsMockable() const override;
    virtual std::string GenerateMock() const override;
};

#endif // header guard
