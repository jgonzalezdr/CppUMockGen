#include "Function.hpp"

#include <stdexcept>
#include <sstream>
#include <string>

#include "ClangHelper.hpp"

//*************************************************************************************************
//
//                                             RETURN PROCESSING
//
//*************************************************************************************************

class Function::Return
{
public:
    virtual ~Return() {}

    void SetOriginalType( const std::string &type )
    {
        m_originalType = type;
    }

    void MockRetExprPrepend( const std::string &expr )
    {
        m_mockRetExprFront.insert( 0, expr );
    }

    void MockRetExprAppend( const std::string &expr )
    {
        m_mockRetExprBack.append( expr );
    }

    std::string GetMockSignature()
    {
        return m_originalType;
    }

    virtual std::string GetMockBodyFront() const = 0;

    virtual std::string GetMockBodyBack() const = 0;

protected:
    std::string m_originalType;
    std::string m_mockRetExprFront;
    std::string m_mockRetExprBack;
};

class ReturnVoid : public Function::Return
{
public:
    virtual ~ReturnVoid() {};

    virtual std::string GetMockBodyFront() const override
    {
        return "";
    }

    virtual std::string GetMockBodyBack() const override
    {
        return "";
    }
};

class ReturnStandard : public Function::Return
{
public:
    virtual ~ReturnStandard() {};

    virtual std::string GetMockBodyFront() const override
    {
        return "return " + m_mockRetExprFront;
    }

    virtual std::string GetMockBodyBack() const override
    {
        return GetMockCall() + m_mockRetExprBack;
    }

    virtual std::string GetMockCall() const = 0;
};

class ReturnBool : public ReturnStandard
{
public:
    virtual ~ReturnBool() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnBoolValue()";
    }
};

class ReturnInt : public ReturnStandard
{
public:
    virtual ~ReturnInt() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnIntValue()";
    }
};

class ReturnUnsignedInt : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedInt() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnUnsignedIntValue()";
    }
};

class ReturnLong : public ReturnStandard
{
public:
    virtual ~ReturnLong() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnLongIntValue()";
    }
};

class ReturnUnsignedLong : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedLong() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnUnsignedLongIntValue()";
    }
};

class ReturnDouble : public ReturnStandard
{
public:
    virtual ~ReturnDouble() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnDoubleValue()";
    }
};

class ReturnString : public ReturnStandard
{
public:
    virtual ~ReturnString() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnStringValue()";
    }
};

class ReturnConstPointer : public ReturnStandard
{
public:
    virtual ~ReturnConstPointer() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnConstPointerValue()";
    }
};

class ReturnPointer : public ReturnStandard
{
public:
    virtual ~ReturnPointer() {};

    virtual std::string GetMockCall() const override
    {
        return ".returnPointerValue()";
    }
};

class ReturnParser
{
public:
    ReturnParser( const Config &config )
    : m_config( config )
    {}

    Function::Return* Process( const std::string funcName, const CXType &returnType );

private:
    Function::Return* ProcessOverride( const Config::OverrideSpec *override );
    Function::Return* ProcessType( const CXType &returnType, bool inheritConst, bool enableCast );
    Function::Return* ProcessTypePointer( const CXType & returnType, bool enableCast );
    Function::Return* ProcessTypeRVReference( const CXType & returnType );
    Function::Return* ProcessTypeTypedef( const CXType & returnType, bool inheritConst );

    const Config &m_config;
};

Function::Return* ReturnParser::Process( const std::string funcName, const CXType &returnType )
{
    Function::Return *ret;

    std::string typeExpr = toString( clang_getTypeSpelling( returnType ) );

    if( returnType.kind == CXType_Void )
    {
        ret = new ReturnVoid;
    }
    else
    {
        // Check if a function specific override exists
        std::string overrideKey = funcName + "@";
        const Config::OverrideSpec *override = m_config.GetParameterOverride( overrideKey );

        if( override == NULL )
        {
            // Check if a generic override exists
            overrideKey = "@" + typeExpr;
            override = m_config.GetTypeOverride( overrideKey );
        }

        if( override == NULL )
        {
            ret = ProcessType( returnType, false, true );
        }
        else
        {
            ret = ProcessOverride( override );
        }
    }

    ret->SetOriginalType( typeExpr );

    return ret;
}

Function::Return* ReturnParser::ProcessOverride( const Config::OverrideSpec *override )
{
    Function::Return *ret;
    MockedType overrideType = override->GetType();

    switch( overrideType )
    {
        case MockedType::Bool:
            ret = new ReturnBool;
            break;

        case MockedType::Int:
            ret = new ReturnInt;
            break;

        case MockedType::UnsignedInt:
            ret = new ReturnUnsignedInt;
            break;

        case MockedType::Long:
            ret = new ReturnLong;
            break;

        case MockedType::UnsignedLong:
            ret = new ReturnUnsignedLong;
            break;

        case MockedType::Double:
            ret = new ReturnDouble;
            break;

        case MockedType::String:
            ret = new ReturnString;
            break;

        case MockedType::Pointer:
            ret = new ReturnPointer;
            break;

        case MockedType::ConstPointer:
            ret = new ReturnConstPointer;
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "<INTERNAL ERROR> Unsupported mocked return type override" );
// LCOV_EXCL_STOP
    }

    ret->MockRetExprPrepend( override->GetExprModFront() );
    ret->MockRetExprAppend( override->GetExprModBack() );

    return ret;
}

Function::Return* ReturnParser::ProcessType( const CXType &returnType, bool inheritConst, bool enableCast )
{
    Function::Return *ret;

    bool needsCast = false;

    switch( returnType.kind )
    {
        case CXType_Bool:
            ret = new ReturnBool;
            break;

        case CXType_Int:
            ret = new ReturnInt;
            break;

        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Short:
        case CXType_WChar:
        case CXType_Enum:
            needsCast = true;
            ret = new ReturnInt;
            break;

        case CXType_UInt:
            ret = new ReturnUnsignedInt;
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Char16:
            needsCast = true;
            ret = new ReturnUnsignedInt;
            break;

        case CXType_Long:
            ret = new ReturnLong;
            break;

        case CXType_ULong:
            ret = new ReturnUnsignedLong;
            break;

        case CXType_Char32:
            needsCast = true;
            ret = new ReturnUnsignedLong;
            break;

        case CXType_Double:
            ret = new ReturnDouble;
            break;

        case CXType_Float:
            needsCast = true;
            ret = new ReturnDouble;
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ret = ProcessTypePointer( returnType, enableCast );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            // Dereference and cast mock return pointer to proper pointer type
            ret = new ReturnConstPointer;
            if( enableCast )
            {
                ret->MockRetExprPrepend( "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( " );
                ret->MockRetExprAppend( " )" );
            }
            break;

        case CXType_Typedef:
            ret = ProcessTypeTypedef( returnType, inheritConst );
            break;

        case CXType_Elaborated:
            ret = ProcessType( clang_Type_getNamedType( returnType ), inheritConst, enableCast );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported return type " + clang_getTypeSpelling( returnType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( returnType.kind) + ")" );
// LCOV_EXCL_STOP
    }

    if( needsCast && enableCast )
    {
        ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( returnType ) + ">( " );
        ret->MockRetExprAppend( " )" );
    }

    return ret;
}

Function::Return* ReturnParser::ProcessTypePointer( const CXType &returnType, bool enableCast )
{
    Function::Return *ret;

    const CXType pointeeType = clang_getPointeeType( returnType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( ( returnType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        ret = new ReturnString;
    }
    else
    {
        // Resolve possible typedefs
        const CXType underlyingPointeeType = clang_getCanonicalType( pointeeType );
        bool isUnderlyingPointeeConst = clang_isConstQualifiedType( underlyingPointeeType );

        if( isPointeeConst || isUnderlyingPointeeConst )
        {
            ret = new ReturnConstPointer;
        }
        else
        {
            ret = new ReturnPointer;
        }

        if( enableCast && ( pointeeType.kind != CXType_Void ) )
        {
            // Cast mock return pointer to proper pointer type
            ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( " );
            ret->MockRetExprAppend( " )" );
        }
    }

    if( returnType.kind == CXType_LValueReference )
    {
        // Dereference mock return pointer
        ret->MockRetExprPrepend( "* " );
    }
    else if( returnType.kind == CXType_RValueReference )
    {
        // Dereference mock return pointer
        ret->MockRetExprPrepend( "std::move( * " );
        ret->MockRetExprAppend( " )" );
    }

    return ret;
}

Function::Return* ReturnParser::ProcessTypeTypedef( const CXType &returnType, bool inheritConst )
{
    Function::Return *ret;

    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Pointer ) ||
        ( underlyingType.kind == CXType_LValueReference ) ||
        ( underlyingType.kind == CXType_RValueReference ) )
    {
        ret = ProcessTypePointer( underlyingType, ( underlyingType.kind != CXType_Pointer ) );
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( returnType ) || inheritConst;

        ret = ProcessType( underlyingType, isTypedefConst, false );
    }

    if( ( underlyingType.kind == CXType_Record ) ||
        ( underlyingType.kind == CXType_Unexposed ) )
    {
        // Dereference mock return pointer
        ret->MockRetExprPrepend( "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( " );
        ret->MockRetExprAppend( " )" );
    }
    else
    {
        // Just cast
        ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( returnType ) + ">( " );
        ret->MockRetExprAppend( " )" );
    }

    return ret;
}


//*************************************************************************************************
//
//                                     ARGUMENT PROCESSING
//
//*************************************************************************************************


class Function::Argument
{
public:
    virtual ~Argument() {}

    void SetName( const std::string &name )
    {
        m_name = name;
    }

    void SetOriginalType( const std::string &type )
    {
        m_originalType = type;
    }

    void MockArgExprPrepend( const std::string &expr )
    {
        m_mockArgExprFront.insert( 0, expr );
    }

    void MockArgExprAppend( const std::string &expr )
    {
        m_mockArgExprBack.append( expr );
    }

    virtual std::string GetSignature(bool mock) const = 0;

    virtual std::string GetBody(bool mock) const = 0;

protected:
    std::string m_name;
    std::string m_originalType;
    std::string m_mockArgExprFront;
    std::string m_mockArgExprBack;
};

class ArgumentSkip : public Function::Argument
{
public:
    virtual ~ArgumentSkip() {}

    virtual std::string GetSignature(bool) const override
    {
        return m_originalType;
    }

    virtual std::string GetBody(bool) const override
    {
        return "";
    }
};

class ArgumentStandard : public Function::Argument
{
public:
    virtual ~ArgumentStandard() {}

    virtual std::string GetSignature(bool) const override
    {
        return m_originalType + " " + m_name;
    }

    virtual std::string GetBody(bool mock) const override
    {
        return GetCallFront(mock) + "\"" + m_name + "\", " + m_mockArgExprFront + m_name + m_mockArgExprBack + GetCallBack(mock);
    }

    virtual std::string GetCallFront(bool mock) const = 0;

    virtual std::string GetCallBack(bool) const
    {
        return ")";
    }
};

class ArgumentBool : public ArgumentStandard
{
public:
    virtual ~ArgumentBool() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withBoolParameter(";
    }
};

class ArgumentInt : public ArgumentStandard
{
public:
    virtual ~ArgumentInt() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withIntParameter(";
    }
};

class ArgumentUnsignedInt : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedInt() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withUnsignedIntParameter(";
    }
};

class ArgumentLong : public ArgumentStandard
{
public:
    virtual ~ArgumentLong() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withLongIntParameter(";
    }
};

class ArgumentUnsignedLong : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedLong() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withUnsignedLongIntParameter(";
    }
};

class ArgumentDouble : public ArgumentStandard
{
public:
    virtual ~ArgumentDouble() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withDoubleParameter(";
    }
};

class ArgumentString : public ArgumentStandard
{
public:
    virtual ~ArgumentString() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withStringParameter(";
    }
};

class ArgumentPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentPointer() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withPointerParameter(";
    }
};

class ArgumentConstPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentConstPointer() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withConstPointerParameter(";
    }
};

class ArgumentOutput : public ArgumentStandard
{
public:
    virtual ~ArgumentOutput() {}

    virtual std::string GetSignature(bool mock) const override
    {
        std::string ret = m_originalType + " " + m_name;
        if( !mock )
        {
            ret += ", size_t __sizeof_" + m_name;
        }
        return ret;
    }

    virtual std::string GetCallFront(bool mock) const override
    {
        if( mock )
        {
            return ".withOutputParameter(";
        }
        else
        {
            return ".withOutputParameterReturning(";
        }
    }

    virtual std::string GetCallBack(bool mock) const
    {
        if( mock )
        {
            return ")";
        }
        else
        {
            return ", __sizeof_" + m_name + ")";
        }
    }
};

class ArgumentOfType : public ArgumentStandard
{
public:
    virtual ~ArgumentOfType() {}

    virtual std::string GetCallFront(bool) const override
    {
        return ".withParameterOfType(\"" + m_exposedType + "\", ";
    }

    void SetExposedType( const std::string &type )
    {
        m_exposedType = type;
    }

protected:
    std::string m_exposedType;
};

class ArgumentOutputOfType : public ArgumentOfType
{
public:
    virtual ~ArgumentOutputOfType() {}

    virtual std::string GetCallFront(bool mock) const override
    {
        if( mock )
        {
            return ".withOutputParameterOfType(\"" + m_exposedType + "\", ";
        }
        else
        {
            return ".withOutputParameterOfTypeReturning(\"" + m_exposedType + "\", ";
        }
    }
};

class ArgumentParser
{
public:
    ArgumentParser( const Config &config )
    : m_config( config )
    {}

    Function::Argument* Process( const std::string funcName, const CXCursor &arg, int argNum );

private:
    Function::Argument* ProcessOverride( const Config::OverrideSpec *override );
    Function::Argument* ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst );
    Function::Argument* ProcessTypePointer( const CXType &argType, const CXType &origArgType );
    Function::Argument* ProcessTypeRVReference( const CXType &argType, const CXType &origArgType );
    Function::Argument* ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst );
    Function::Argument* ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee );

    const Config &m_config;
};

Function::Argument* ArgumentParser::Process( const std::string funcName, const CXCursor &arg, int argNum )
{
    Function::Argument *ret;

    // Get argument type
    const CXType argType = clang_getCursorType( arg );
    std::string typeExpr = toString( clang_getTypeSpelling( argType ) );

    // Get argument name
    std::string argName = toString( clang_getCursorSpelling( arg ) );
    if( argName.empty() )
    {
        std::ostringstream argNameStream;
        argNameStream << "_unnamedArg" << argNum;
        argName = argNameStream.str();
    }

    // Check if a function specific override exists
    std::string overrideKey = funcName + "#" + argName;
    const Config::OverrideSpec *override = m_config.GetParameterOverride( overrideKey );

    if( override == NULL )
    {
        // Check if a generic type override exists
        overrideKey = "#" + typeExpr;
        override = m_config.GetTypeOverride( overrideKey );
    }

    if( override == NULL )
    {
        ret = ProcessType( argType, argType, false );
    }
    else
    {
        ret = ProcessOverride( override );
    }

    ret->SetName( argName );
    ret->SetOriginalType( typeExpr );

    return ret;
}

Function::Argument* ArgumentParser::ProcessOverride( const Config::OverrideSpec *override )
{
    Function::Argument *ret;
    MockedType overrideType = override->GetType();

    switch( overrideType )
    {
        case MockedType::Bool:
            ret = new ArgumentBool;
            break;

        case MockedType::Int:
            ret = new ArgumentInt;
            break;

        case MockedType::UnsignedInt:
            ret = new ArgumentUnsignedInt;
            break;

        case MockedType::Long:
            ret = new ArgumentLong;
            break;

        case MockedType::UnsignedLong:
            ret = new ArgumentUnsignedLong;
            break;

        case MockedType::Double:
            ret = new ArgumentDouble;
            break;

        case MockedType::String:
            ret = new ArgumentString;
            break;

        case MockedType::Pointer:
            ret = new ArgumentPointer;
            break;

        case MockedType::ConstPointer:
            ret = new ArgumentConstPointer;
            break;

        case MockedType::Output:
            ret = new ArgumentOutput;
            break;

        case MockedType::Skip:
            ret = new ArgumentSkip;
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "<INTERNAL ERROR> Unsupported mocked argument type override" );
// LCOV_EXCL_STOP
    }

    if( overrideType != MockedType::Skip )
    {
        ret->MockArgExprPrepend( override->GetExprModFront() );
        ret->MockArgExprAppend( override->GetExprModBack() );
    }

    return ret;
}

Function::Argument* ArgumentParser::ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst )
{
    Function::Argument *ret;

    switch( argType.kind )
    {
        case CXType_Bool:
            ret = new ArgumentBool;
            break;

        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Int:
        case CXType_Short:
        case CXType_WChar:
            ret = new ArgumentInt;
            break;

        case CXType_Enum:
            ret = new ArgumentInt;
            ret->MockArgExprPrepend( "static_cast<int>(" );
            ret->MockArgExprAppend( ")" );
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UInt:
        case CXType_UShort:
        case CXType_Char16:
            ret = new ArgumentUnsignedInt;
            break;

        case CXType_Long:
            ret = new ArgumentLong;
            break;

        case CXType_ULong:
        case CXType_Char32:
            ret = new ArgumentUnsignedLong;
            break;

        case CXType_Float:
        case CXType_Double:
            ret = new ArgumentDouble;
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
            ret = ProcessTypePointer( argType, origArgType );
            break;

        case CXType_RValueReference:
            ret = ProcessTypeRVReference( argType, origArgType );
            break;

        case CXType_Typedef:
            ret = ProcessTypeTypedef( argType, origArgType, inheritConst );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            ret = ProcessTypeRecord( argType, origArgType, inheritConst, false );
            ret->MockArgExprPrepend("&");
            break;

        case CXType_Elaborated:
            ret = ProcessType( clang_Type_getNamedType( argType ), origArgType, inheritConst );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported parameter type " + clang_getTypeSpelling( argType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( argType.kind) + ")" );
            break;
// LCOV_EXCL_STOP
    }

    return ret;
}

Function::Argument* ArgumentParser::ProcessTypePointer( const CXType &argType, const CXType &origArgType )
{
    Function::Argument *ret;

    const CXType pointeeType = clang_getPointeeType( argType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( ( argType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        ret = new ArgumentString;
    }
    else
    {
        // Resolve possible typedefs
        const CXType underlyingPointeeType = clang_getCanonicalType( pointeeType );
        bool isUnderlyingPointeeConst = clang_isConstQualifiedType( underlyingPointeeType );

        if( isPointeeConst || isUnderlyingPointeeConst )
        {
            switch( underlyingPointeeType.kind )
            {
                case CXType_Record:
                case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
                    ret = ProcessTypeRecord( underlyingPointeeType, origArgType, true, true );
                    break;

                default:
                    ret = new ArgumentConstPointer;
                    break;
            }
        }
        else
        {
            switch( underlyingPointeeType.kind )
            {
                case CXType_Void:
                case CXType_Pointer:
                case CXType_LValueReference:
                case CXType_RValueReference:
                    ret = new ArgumentPointer;
                    break;

                case CXType_Record:
                case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
                    ret = ProcessTypeRecord( underlyingPointeeType, origArgType, false, true );
                    break;

                default:
                    ret = new ArgumentOutput;
                    break;
            }
        }
    }

    if( argType.kind != CXType_Pointer )
    {
        ret->MockArgExprPrepend("&");
    }

    return ret;
}

Function::Argument* ArgumentParser::ProcessTypeRVReference( const CXType &argType, const CXType &origArgType )
{
    Function::Argument *ret;

    CXType pointeeType = clang_getPointeeType( argType );

    // Remove sugar like "struct", etc.
    if( pointeeType.kind == CXType_Elaborated )
    {
        pointeeType = clang_Type_getNamedType( pointeeType );
    }

    if( ( pointeeType.kind == CXType_Record ) ||
        ( pointeeType.kind == CXType_Unexposed ) )
    {
        ret = ProcessTypePointer( argType, origArgType );
    }
    else
    {
        bool isPointeeConst = clang_isConstQualifiedType( pointeeType );
        ret = ProcessType( pointeeType, origArgType, isPointeeConst );
    }

    return ret;
}

Function::Argument* ArgumentParser::ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst )
{
    Function::Argument *ret;

    const CXType underlyingType = clang_getCanonicalType( argType );

    if( ( underlyingType.kind == CXType_Pointer ) ||
        ( underlyingType.kind == CXType_LValueReference ) ||
        ( underlyingType.kind == CXType_RValueReference ) )
    {
        const CXType pointeeType = clang_getPointeeType( underlyingType );
        bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

        if( ( underlyingType.kind == CXType_Pointer ) &&
            ( pointeeType.kind == CXType_Char_S ) &&
            isPointeeConst )
        {
            ret = new ArgumentString;
        }
        else if( isPointeeConst )
        {
            ret = new ArgumentConstPointer;
        }
        else
        {
            ret = new ArgumentPointer;
        }

        if( underlyingType.kind != CXType_Pointer )
        {
            ret->MockArgExprPrepend("&");
        }
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( argType ) || inheritConst;
        ret = ProcessType( underlyingType, origArgType, isTypedefConst );
    }

    return ret;
}

Function::Argument* ArgumentParser::ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee )
{
    ArgumentOfType *ret;

    bool isConst = clang_isConstQualifiedType( argType ) || inheritConst;
    if( !isPointee || isConst )
    {
        ret = new ArgumentOfType;
    }
    else
    {
        ret = new ArgumentOutputOfType;
    }
    ret->SetExposedType( getBareTypeSpelling( m_config.UseUnderlyingTypedefType() ? argType : origArgType ) );

    return ret;
}


//*************************************************************************************************
//
//                                        FUNCTION PROCESSING
//
//*************************************************************************************************

Function::Function()
: m_isConst( false )
{
}

// LCOV_EXCL_START
Function::~Function()
{
}
// LCOV_EXCL_STOP

bool Function::IsMockable( const CXCursor &cursor ) const
{
    // If the function has a definition, it means that it's defined inline, and therefore it's not mockable
    bool hasNoDefinition = clang_Cursor_isNull( clang_getCursorDefinition( cursor ) );

    // Only consider canonical cursors to avoid generating several mocks for the same function if it's
    // declared more than once
    bool isCanonical = clang_equalCursors( cursor, clang_getCanonicalCursor( cursor ) );

    return isCanonical && hasNoDefinition;
}

bool Function::Parse( const CXCursor &cursor, const Config &config )
{
    if( IsMockable(cursor) )
    {
        ReturnParser returnParser( config );
        ArgumentParser argumentParser( config );

        // Get function name
        m_functionName = getQualifiedName( cursor );

        // Get method constantness
        m_isConst = ( IsMethod() && clang_CXXMethod_isConst(cursor) );

        // Get & process function return type
        const CXType returnType = clang_getCursorResultType( cursor );
        m_return = std::unique_ptr<Return>( returnParser.Process( m_functionName, returnType ) );

        // Process arguments
        int numArgs = clang_Cursor_getNumArguments( cursor );
        for( int i = 0; i < numArgs; i++ )
        {
            const CXCursor arg = clang_Cursor_getArgument( cursor, i );
            m_arguments.push_back( std::unique_ptr<Argument>( argumentParser.Process( m_functionName, arg, i ) ) );
        }

        return true;
    }
    else
    {
        return false;
    }
}

std::string Function::GenerateMock() const
{
// LCOV_EXCL_START
    if( m_functionName.empty() )
    {
        return "";
    }
// LCOV_EXCL_STOP

    std::string signature = m_return->GetMockSignature() + " " + m_functionName + "(";

    std::string body = m_return->GetMockBodyFront() + "mock().actualCall(\"" + m_functionName + "\")";

    if( IsMethod() )
    {
        body += ".onObject(this)";
    }

    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        if( i > 0 )
        {
            signature += ", ";
        }

        signature += m_arguments[i]->GetSignature(true);

        body += m_arguments[i]->GetBody(true);
    }

    signature += ")";

    if( m_isConst )
    {
        signature += " const";
    }

    body += m_return->GetMockBodyBack();

    return signature + "\n{\n    " + body + ";\n}\n";
}

static std::string namespaceSeparator = "::";

static std::vector<std::string> GetNamespaceDecomposition( const std::string &qualifiedName )
{
    std::vector<std::string> ret;

    size_t initPos = 0;
    size_t endPos = 0;

    while( endPos != std::string::npos )
    {
        endPos = qualifiedName.find(namespaceSeparator, initPos);
        if( endPos != std::string::npos )
        {
            ret.push_back( qualifiedName.substr( initPos, (endPos - initPos) ) );
        }
        else
        {
            ret.push_back( qualifiedName.substr( initPos ) );
        }
        initPos = endPos + namespaceSeparator.size();
    }

    return ret;
}

std::string Function::GenerateExpectation( bool proto ) const
{
// LCOV_EXCL_START
    if( m_functionName.empty() )
    {
        return "";
    }
// LCOV_EXCL_STOP

    // Namespace opening
    std::string ret = "namespace expect {";

    std::vector<std::string> namespaces = GetNamespaceDecomposition( m_functionName );
    for( size_t i = 0; i < (namespaces.size() - 1); i++ )
    {
        ret += " namespace " + namespaces[i] + " {";
    }
    ret += "\n";

    // Function processing
    ret += GenerateExpectation( proto, namespaces[namespaces.size()-1], true );
    ret += GenerateExpectation( proto, namespaces[namespaces.size()-1], false );

    // Namespace closing
    for( size_t i = 0; i < (namespaces.size() - 1); i++ )
    {
        ret += "} ";
    }
    ret += "}\n";

    return ret;
}

std::string Function::GenerateExpectation( bool proto, std::string functionName, bool oneCall ) const
{
    std::string ret = "void " + functionName + "(";

    if( !oneCall )
    {
        ret += "unsigned int __numCalls__";
    }

    if( IsMethod() )
    {
        if( !oneCall )
        {
            ret += ", ";
        }
        ret += "void *__object__";
    }

    std::string body;
    if( !proto )
    {
        if( oneCall )
        {
            body = "mock().expectOneCall(\"" + m_functionName + "\")";
        }
        else
        {
            body = "mock().expectNCalls(__numCalls__, \"" + m_functionName + "\")";
        }

        if( IsMethod() )
        {
            body += ".onObject(__object__)";
        }
    }

    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        if( ( i > 0 ) || IsMethod() || !oneCall )
        {
            ret += ", ";
        }

        ret += m_arguments[i]->GetSignature(false);

        if( !proto )
        {
            body += m_arguments[i]->GetBody(false);
        }
    }

    if( proto )
    {
        ret += ");\n";
    }
    else
    {
        ret += ")\n{\n    " + body + ";\n}\n";
    }

    return ret;
}
