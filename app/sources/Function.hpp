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

    class Mock
    {
    public:
        std::string signature;
        std::string body;
    };

    std::string GenerateMock(bool isMethod) const;
    unsigned int ProcessReturnInitial( const CXType &returnType, Mock &mock ) const;
    unsigned int ProcessReturnInitialType( const CXType &returnType, Mock &mock ) const;
    unsigned int ProcessReturnInitialTypePointer( const CXType & returnType, Mock &mock ) const;
    unsigned int ProcessReturnInitialTypeRVReference( const CXType & returnType, Mock &mock ) const;
    unsigned int ProcessReturnInitialTypeTypedef( const CXType & returnType, Mock &mock ) const;
    void ProcessReturnFinal( const CXType &returnType, bool inheritConst, Mock &mock ) const;
    void ProcessReturnFinalTypePointer( const CXType &returnType, Mock &mock ) const;
    void ProcessReturnFinalTypeRVReference( const CXType &returnType, Mock &mock ) const;
    void ProcessReturnFinalTypeTypedef( const CXType &returnType, bool inheritConst, Mock &mock ) const;
    void ProcessArgument( const std::string funcName, const CXCursor &arg, int argNum, Mock &mock ) const;
    void ProcessArgumentOverride( const Config::OverrideSpec& overrideSpec, const std::string& argId, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypePointer( const CXType &argType, const CXType &origArgType, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypeRVReference( const CXType &argType, const CXType &origArgType, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const;
    void ProcessArgumentTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee, Mock &mock ) const;
};

#endif // header guard
