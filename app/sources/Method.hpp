#ifndef METHOD_HPP_
#define METHOD_HPP_

#include "Function.hpp"

/**
 * Class used to parse methods and generate mocks.
 */
class Method : public Function
{
public:
    /**
     * Default constructor.
     */
    Method();

protected:
    virtual bool IsMockable( const CXCursor &cursor ) const override;

    virtual bool IsMethod() const override
    {
        return true;
    }
};

#endif // header guard
