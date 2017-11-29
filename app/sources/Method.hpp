#ifndef METHOD_HPP_
#define METHOD_HPP_

#include "Function.hpp"

class Method : public Function
{
public:
    Method( const CXCursor &cursor, const Config &config );

    virtual bool IsMockable() const override;
    virtual std::string GenerateMock() const override;
};

#endif // header guard
