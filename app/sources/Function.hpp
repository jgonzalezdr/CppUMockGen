#ifndef FUNCTION_HPP_
#define FUNCTION_HPP_

#include <clang-c/Index.h>
#include <string>
#include "Config.hpp"

class Function
{
public:
    Function( const CXCursor &cursor, const Config &config );
    ~Function();

    bool IsMockable() const;

    std::string GenerateMock() const;

protected:
    const CXCursor &m_cursor;
    const Config &m_config;

    class Mock
    {
    public:
        std::string signature;
        std::string body;
    };

    class UnderlyingType
    {
    public:
        const CXType type;
        bool isConst;
    };



    unsigned int ProcessReturnTypeInitial( const CXType &returnType, Mock &mock ) const;
    unsigned int ProcessReturnTypeInitialPointer( const CXType & returnType, Mock &mock ) const;
    unsigned int ProcessReturnTypeInitialTypedef( const CXType & returnType, Mock &mock ) const;
    void ProcessReturnTypeFinal( const CXType &returnType, Mock &mock, bool inheritConst, unsigned int castsPerformed ) const;
    void ProcessReturnTypeFinalPointer( const CXType &returnType, Mock &mock ) const;
    void ProcessReturnTypeFinalTypedef( const CXType &returnType, Mock &mock, bool inheritConst ) const;
    void ProcessArgument( const CXCursor &arg, Mock &mock ) const;
    void ProcessArgumentType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypePointer( const CXType &argType, const CXType &origArgType, Mock &mock ) const;
    void ProcessArgumentTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee, Mock &mock ) const;
    //void ProcessArgumentTypePointedTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const;
    //void ProcessArgumentTypePointedType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const;
};

#endif // header guard
