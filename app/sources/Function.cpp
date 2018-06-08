#include "Function.hpp"

#include <stdexcept>
#include <sstream>
#include <string>

#include "ClangHelper.hpp"

//*************************************************************************************************
//                                             CONSTANTS
//*************************************************************************************************

#define RETURN_ARG_NAME         "__return__"
#define OBJECT_ARG_NAME         "__object__"
#define NUM_CALLS_ARG_NAME      "__numCalls__"
#define EXPECTED_CALL_VAR_NAME  "__expectedCall__"
#define IGNORE_OTHERS_VAR_NAME  "__ignoreOtherParams__"
#define SIZEOF_VAR_PREFIX       "__sizeof_"
#define INDENT                  "    "

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

    std::string GetMockSignature() const
    {
        return m_originalType;
    }

    virtual std::string GetMockBodyFront() const = 0;

    virtual std::string GetMockBodyBack() const = 0;

    virtual std::string GetExpectationSignature() const = 0;

    virtual std::string GetExpectationBody() const = 0;

protected:
    std::string m_originalType;
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

    virtual std::string GetExpectationSignature() const override
    {
        return "";
    }

    virtual std::string GetExpectationBody() const override
    {
        return "";
    }
};

class ReturnStandard : public Function::Return
{
public:
    ReturnStandard() : m_expectationArgByRef(false), m_expectationNeedsCast(false), m_isRVReference(false) {}
    virtual ~ReturnStandard() {};

    void BothRetExprPrepend( const std::string &expr )
    {
        m_mockRetExprFront.insert( 0, expr );
        m_expectationRetExprFront.insert( 0, expr );
    }

    void BothRetExprAppend( const std::string &expr )
    {
        m_mockRetExprBack.append( expr );
        m_expectationRetExprBack.append( expr );
    }

    void MockRetExprPrepend( const std::string &expr )
    {
        m_mockRetExprFront.insert( 0, expr );
    }

    void MockRetExprAppend( const std::string &expr )
    {
        m_mockRetExprBack.append( expr );
    }

    void ExpectationRetExprPrepend( const std::string &expr )
    {
        m_expectationRetExprFront.insert( 0, expr );
    }

    void ExpectationRetExprAppend( const std::string &expr )
    {
        m_expectationRetExprBack.append( expr );
    }

    virtual std::string GetMockBodyFront() const override
    {
        return "return " + m_mockRetExprFront;
    }

    virtual std::string GetMockBodyBack() const override
    {
        return "." + GetMockCall() + "()" + m_mockRetExprBack;
    }

    virtual std::string GetExpectationSignature() const override
    {
        return GetSignatureType() + (m_expectationArgByRef ? " &" : " ") + RETURN_ARG_NAME;
    }

    virtual std::string GetExpectationBody() const override
    {
        return INDENT EXPECTED_CALL_VAR_NAME ".andReturnValue(" + GetExpectationRetExprFront() +
               RETURN_ARG_NAME +  GetExpectationRetExprBack() + ");\n";
    }

    void SetExpectationArgByRef()
    {
        m_expectationArgByRef = true;
    }

    void SetExpectationNeedsCast()
    {
        m_expectationNeedsCast = true;
    }

    void SetIsRVReference()
    {
        m_isRVReference = true;
    }

protected:
    virtual std::string GetMockCall() const = 0;

    virtual std::string GetBaseType() const = 0;

private:
    std::string GetExpectationRetExprFront() const
    {
        return ( m_expectationNeedsCast ? "static_cast<" + GetBaseType() + ">(" : "" ) + m_expectationRetExprFront;
    }

    std::string GetExpectationRetExprBack() const
    {
        return m_expectationRetExprBack +  ( m_expectationNeedsCast ? ")" : "" );
    }

    std::string GetSignatureType() const
    {
        if( m_isRVReference )
        {
            return m_originalType.substr( 0, ( m_originalType.size() - 1 ) );
        }
        else
        {
            return m_originalType;
        }
    }

    std::string m_mockRetExprFront;
    std::string m_mockRetExprBack;
    std::string m_expectationRetExprFront;
    std::string m_expectationRetExprBack;
    bool m_expectationArgByRef;
    bool m_expectationNeedsCast;
    bool m_isRVReference;
};

class ReturnBool : public ReturnStandard
{
public:
    virtual ~ReturnBool() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "bool";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnBoolValue";
    }
};

class ReturnInt : public ReturnStandard
{
public:
    virtual ~ReturnInt() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "int";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnIntValue";
    }
};

class ReturnUnsignedInt : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedInt() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "unsigned int";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnUnsignedIntValue";
    }
};

class ReturnLong : public ReturnStandard
{
public:
    virtual ~ReturnLong() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "long";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnLongIntValue";
    }
};

class ReturnUnsignedLong : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedLong() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "unsigned long";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnUnsignedLongIntValue";
    }
};

class ReturnDouble : public ReturnStandard
{
public:
    virtual ~ReturnDouble() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "double";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnDoubleValue";
    }
};

class ReturnString : public ReturnStandard
{
public:
    virtual ~ReturnString() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "const char*";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnStringValue";
    }
};

class ReturnConstPointer : public ReturnStandard
{
public:
    virtual ~ReturnConstPointer() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "const void*";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnConstPointerValue";
    }
};

class ReturnPointer : public ReturnStandard
{
public:
    virtual ~ReturnPointer() {};

protected:
    virtual std::string GetBaseType() const override
    {
        return "void*";
    }

    virtual std::string GetMockCall() const override
    {
        return "returnPointerValue";
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
    ReturnStandard* ProcessType( const CXType &returnType, bool inheritConst, bool enableCast );
    ReturnStandard* ProcessTypePointer( const CXType & returnType, bool enableCast );
    ReturnStandard* ProcessTypeRVReference( const CXType & returnType );
    ReturnStandard* ProcessTypeTypedef( const CXType & returnType, bool inheritConst );

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
    ReturnStandard *ret;
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

    ret->BothRetExprPrepend( override->GetExprModFront() );
    ret->BothRetExprAppend( override->GetExprModBack() );
    ret->SetExpectationNeedsCast();

    return ret;
}

ReturnStandard* ReturnParser::ProcessType( const CXType &returnType, bool inheritConst, bool enableCast )
{
    ReturnStandard *ret;

    bool mockNeedsCast = false;

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
            ret = new ReturnInt;
            ret->SetExpectationNeedsCast();
            mockNeedsCast = true;
            break;

        case CXType_UInt:
            ret = new ReturnUnsignedInt;
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Char16:
            ret = new ReturnUnsignedInt;
            ret->SetExpectationNeedsCast();
            mockNeedsCast = true;
            break;

        case CXType_Long:
            ret = new ReturnLong;
            break;

        case CXType_ULong:
            ret = new ReturnUnsignedLong;
            break;

        case CXType_Char32:
            ret = new ReturnUnsignedLong;
            ret->SetExpectationNeedsCast();
            mockNeedsCast = true;
            break;

        case CXType_Double:
            ret = new ReturnDouble;
            break;

        case CXType_Float:
            ret = new ReturnDouble;
            ret->SetExpectationNeedsCast();
            mockNeedsCast = true;
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
            ret->SetExpectationNeedsCast();
            ret->SetExpectationArgByRef();
            if( enableCast )
            {
                ret->MockRetExprPrepend( "*static_cast<const " + clang_getTypeSpelling( returnType ) + "*>(" );
                ret->MockRetExprAppend( ")" );
                ret->ExpectationRetExprPrepend( "&" );
            }
            break;

        case CXType_Typedef:
            ret = ProcessTypeTypedef( returnType, inheritConst );
            ret->SetExpectationNeedsCast();
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

    if( mockNeedsCast && enableCast )
    {
        ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( returnType ) + ">(" );
        ret->MockRetExprAppend( ")" );
    }

    return ret;
}

ReturnStandard* ReturnParser::ProcessTypePointer( const CXType &returnType, bool enableCast )
{
    ReturnStandard *ret;

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

        if( pointeeType.kind != CXType_Void )
        {
            ret->SetExpectationNeedsCast();

            if( enableCast )
            {
                // Cast mock return pointer to proper pointer type
                ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( pointeeType ) + "*>(" );
                ret->MockRetExprAppend( ")" );
            }
        }
    }

    if( returnType.kind == CXType_LValueReference )
    {
        // Dereference mock return pointer
        ret->MockRetExprPrepend( "*" );
        ret->ExpectationRetExprPrepend( "&" );
    }
    else if( returnType.kind == CXType_RValueReference )
    {
        // Dereference mock return pointer
        ret->MockRetExprPrepend( "std::move(*" );
        ret->MockRetExprAppend( ")" );
        ret->ExpectationRetExprPrepend( "&" );
        ret->SetIsRVReference();
    }

    return ret;
}

ReturnStandard* ReturnParser::ProcessTypeTypedef( const CXType &returnType, bool inheritConst )
{
    ReturnStandard *ret;

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
        ret->MockRetExprPrepend( "*static_cast<const " + clang_getTypeSpelling( returnType ) + "*>(" );
        ret->MockRetExprAppend( ")" );
        ret->SetExpectationArgByRef();
        ret->ExpectationRetExprPrepend( "&" );
    }
    else
    {
        // Just cast
        ret->MockRetExprPrepend( "static_cast<" + clang_getTypeSpelling( returnType ) + ">(" );
        ret->MockRetExprAppend( ")" );
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

    virtual std::string GetSignature( bool mock ) const = 0;

    virtual std::string GetBody( bool mock, bool argumentsSkipped ) const = 0;

    virtual bool CanBeIgnored() const = 0;

    virtual bool IsSkipped() const = 0;

protected:
    std::string m_name;
    std::string m_originalType;
};

class ArgumentSkip : public Function::Argument
{
public:
    virtual ~ArgumentSkip() {}

    virtual std::string GetSignature( bool mock ) const override
    {
        return mock ? m_originalType : "";
    }

    virtual std::string GetBody( bool, bool ) const override
    {
        return "";
    }

    virtual bool IsSkipped() const
    {
        return true;
    }

    virtual bool CanBeIgnored() const override
    {
        return false;
    }
};

class ArgumentStandard : public Function::Argument
{
public:
    ArgumentStandard() : m_expectationArgByRef(false), m_forceNotIgnored(false), m_isRVReference(false) {}
    virtual ~ArgumentStandard() {}

    virtual std::string GetSignature( bool mock ) const override
    {
        if( mock )
        {
            return m_originalType + " " + m_name;
        }
        else if( CanBeIgnored() )
        {
            return "CppUMockGen::Parameter<" + m_originalType + (m_expectationArgByRef ? "&> " : "> ") + m_name;
        }
        else
        {
            return GetSignatureType() + (m_expectationArgByRef ? " &" : " ") + m_name;
        }
    }

    virtual std::string GetBody( bool mock, bool argumentsSkipped ) const override
    {
        if( mock )
        {
            return "." + GetBodyCall(mock, "");
        }
        else if( CanBeIgnored() )
        {
            if( argumentsSkipped )
            {
                return INDENT "if(!" + m_name + ".isIgnored()) { " EXPECTED_CALL_VAR_NAME "." + GetBodyCall(mock, ".getValue()") + "; }\n";
            }
            else
            {
                return INDENT "if(" + m_name + ".isIgnored()) { " IGNORE_OTHERS_VAR_NAME " = true; } else { " EXPECTED_CALL_VAR_NAME "." +
                       GetBodyCall(mock, ".getValue()") + "; }\n";
            }
        }
        else
        {
            return INDENT EXPECTED_CALL_VAR_NAME "." + GetBodyCall(mock, "") + ";\n";
        }
    }

    virtual bool IsSkipped() const
    {
        return false;
    }

    void MockArgExprPrepend( const std::string &expr )
    {
        m_mockArgExprFront.insert( 0, expr );
    }

    void MockArgExprAppend( const std::string &expr )
    {
        m_mockArgExprBack.append( expr );
    }

    void SetExpectationArgByRef()
    {
        m_expectationArgByRef = true;
    }

    virtual bool CanBeIgnored() const override
    {
        return !m_forceNotIgnored && isInput();
    }

    void ForceNotIgnored()
    {
        m_forceNotIgnored = true;
    }

    void SetIsRVReference()
    {
        m_isRVReference = true;
    }

protected:
    virtual bool isInput() const
    {
        return true;
    }

    virtual std::string GetCallFront(bool mock) const = 0;

    virtual std::string GetCallBack(bool) const
    {
        return ")";
    }

private:
    std::string GetBodyCall(bool mock, const std::string& getter) const
    {
        return GetCallFront(mock) + "\"" + m_name + "\", " + m_mockArgExprFront + m_name + getter + m_mockArgExprBack + GetCallBack(mock);
    }

    std::string GetSignatureType() const
    {
        if( m_isRVReference )
        {
            return m_originalType.substr( 0, ( m_originalType.size() - 1 ) );
        }
        else
        {
            return m_originalType;
        }
    }

    std::string m_mockArgExprFront;
    std::string m_mockArgExprBack;
    bool m_expectationArgByRef;
    bool m_forceNotIgnored;
    bool m_isRVReference;
};

class ArgumentBool : public ArgumentStandard
{
public:
    virtual ~ArgumentBool() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withBoolParameter(";
    }
};

class ArgumentInt : public ArgumentStandard
{
public:
    virtual ~ArgumentInt() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withIntParameter(";
    }
};

class ArgumentUnsignedInt : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedInt() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withUnsignedIntParameter(";
    }
};

class ArgumentLong : public ArgumentStandard
{
public:
    virtual ~ArgumentLong() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withLongIntParameter(";
    }
};

class ArgumentUnsignedLong : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedLong() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withUnsignedLongIntParameter(";
    }
};

class ArgumentDouble : public ArgumentStandard
{
public:
    virtual ~ArgumentDouble() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withDoubleParameter(";
    }
};

class ArgumentString : public ArgumentStandard
{
public:
    virtual ~ArgumentString() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withStringParameter(";
    }
};

class ArgumentPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentPointer() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withPointerParameter(";
    }
};

class ArgumentConstPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentConstPointer() {}

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withConstPointerParameter(";
    }
};

class ArgumentOutput : public ArgumentStandard
{
public:
    ArgumentOutput() : m_calculateSizeFromType(false) {}
    virtual ~ArgumentOutput() {}

    virtual std::string GetSignature(bool mock) const override
    {
        std::string ret = m_originalType + " " + m_name;
        if( !mock && !m_calculateSizeFromType )
        {
            ret += ", size_t " SIZEOF_VAR_PREFIX + m_name;
        }
        return ret;
    }

    void CalculateSizeFromType()
    {
        m_calculateSizeFromType = true;
    }

protected:
    virtual bool isInput() const override
    {
        return false;
    }

    virtual std::string GetCallFront(bool mock) const override
    {
        if( mock )
        {
            return "withOutputParameter(";
        }
        else
        {
            return "withOutputParameterReturning(";
        }
    }

    virtual std::string GetCallBack(bool mock) const
    {
        if( mock )
        {
            return ")";
        }
        else if( m_calculateSizeFromType )
        {
            return ", sizeof(" + m_name + "))";
        }
        else
        {
            return ", " SIZEOF_VAR_PREFIX + m_name + ")";
        }
    }

    bool m_calculateSizeFromType;
};

class ArgumentOfType : public ArgumentStandard
{
public:
    virtual ~ArgumentOfType() {}

    void SetExposedType( const std::string &type )
    {
        m_exposedType = type;
    }

protected:
    virtual std::string GetCallFront(bool) const override
    {
        return "withParameterOfType(\"" + m_exposedType + "\", ";
    }

    std::string m_exposedType;
};

class ArgumentOutputOfType : public ArgumentOfType
{
public:
    virtual ~ArgumentOutputOfType() {}

protected:
    virtual bool isInput() const override
    {
        return false;
    }

    virtual std::string GetCallFront(bool mock) const override
    {
        if( mock )
        {
            return "withOutputParameterOfType(\"" + m_exposedType + "\", ";
        }
        else
        {
            return "withOutputParameterOfTypeReturning(\"" + m_exposedType + "\", ";
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
    ArgumentStandard* ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst );
    ArgumentStandard* ProcessTypePointer( const CXType &argType, const CXType &origArgType );
    ArgumentStandard* ProcessTypeRVReference( const CXType &argType, const CXType &origArgType );
    ArgumentStandard* ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst );
    ArgumentStandard* ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee );

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
    ArgumentStandard *ret;
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
            return new ArgumentSkip;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "<INTERNAL ERROR> Unsupported mocked argument type override" );
// LCOV_EXCL_STOP
    }

    ret->MockArgExprPrepend( override->GetExprModFront() );
    ret->MockArgExprAppend( override->GetExprModBack() );

    return ret;
}

ArgumentStandard* ArgumentParser::ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst )
{
    ArgumentStandard *ret;

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
            ret->SetExpectationArgByRef();
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

ArgumentStandard* ArgumentParser::ProcessTypePointer( const CXType &argType, const CXType &origArgType )
{
    ArgumentStandard *ret;

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
                    ret = new ArgumentPointer;
                    break;

                case CXType_RValueReference:
                    ret = new ArgumentPointer;
                    ret->ForceNotIgnored();
                    break;

                case CXType_Record:
                case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
                    ret = ProcessTypeRecord( underlyingPointeeType, origArgType, false, true );
                    break;

                default:
                    ret = new ArgumentOutput;
                    if( argType.kind != CXType_Pointer )
                    {
                        static_cast<ArgumentOutput*>(ret)->CalculateSizeFromType();
                    }
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

ArgumentStandard* ArgumentParser::ProcessTypeRVReference( const CXType &argType, const CXType &origArgType )
{
    ArgumentStandard *ret;

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

    ret->ForceNotIgnored();
    ret->SetIsRVReference();
    return ret;
}

ArgumentStandard* ArgumentParser::ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst )
{
    ArgumentStandard *ret;

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

        if( underlyingType.kind == CXType_RValueReference )
        {
            ret->ForceNotIgnored();
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

ArgumentStandard* ArgumentParser::ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee )
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

        signature += m_arguments[i]->GetSignature( true );

        body += m_arguments[i]->GetBody( true, false );
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
        ret += " namespace " + namespaces[i] + "$ {";
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
    bool addSignatureSeparator = false;
    bool argumentsSkipped = hasSkippedArguments();
    bool checkIgnoredArguments = !argumentsSkipped && hasIgnorableArguments();

    std::string ret = "MockExpectedCall& " + functionName + "(";

    if( !oneCall )
    {
        ret += "unsigned int " NUM_CALLS_ARG_NAME;
        addSignatureSeparator = true;
    }

    if( IsMethod() )
    {
        if( addSignatureSeparator )
        {
            ret += ", ";
        }

        ret += "CppUMockGen::Parameter<void*> " OBJECT_ARG_NAME;
        addSignatureSeparator = true;
    }

    std::string body;
    if( !proto )
    {
        if( checkIgnoredArguments )
        {
            body += INDENT "bool " IGNORE_OTHERS_VAR_NAME " = false;\n";
        }

        if( oneCall )
        {
            body += INDENT "MockExpectedCall& " EXPECTED_CALL_VAR_NAME " = mock().expectOneCall(\"" + m_functionName + "\");\n";
        }
        else
        {
            body += INDENT "MockExpectedCall& " EXPECTED_CALL_VAR_NAME " = mock().expectNCalls(" NUM_CALLS_ARG_NAME ", \"" + m_functionName + "\");\n";
        }

        if( IsMethod() )
        {
            body += INDENT "if(!" OBJECT_ARG_NAME ".isIgnored()) { " EXPECTED_CALL_VAR_NAME ".onObject(" OBJECT_ARG_NAME ".getValue()); }\n";
        }
    }

    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        std::string argumentSignature = m_arguments[i]->GetSignature(false);

        if( !argumentSignature.empty() )
        {
            if( addSignatureSeparator )
            {
                ret += ", ";
            }

            ret += argumentSignature;
            addSignatureSeparator = true;
        }

        if( !proto )
        {
            body += m_arguments[i]->GetBody( false, argumentsSkipped );
        }
    }

    std::string returnSignature = m_return->GetExpectationSignature();

    if( !returnSignature.empty() )
    {
        if( addSignatureSeparator )
        {
            ret += ", ";
        }

        ret += returnSignature;
    }

    if( proto )
    {
        ret += ");\n";
    }
    else
    {
        body += m_return->GetExpectationBody();

        if( argumentsSkipped )
        {
            body += INDENT EXPECTED_CALL_VAR_NAME ".ignoreOtherParameters();\n";
        }
        else if( checkIgnoredArguments )
        {
            body += INDENT "if(" IGNORE_OTHERS_VAR_NAME ") { " EXPECTED_CALL_VAR_NAME ".ignoreOtherParameters(); }\n";
        }
        body += INDENT "return " EXPECTED_CALL_VAR_NAME ";";

        ret += ")\n{\n" + body + "\n}\n";
    }

    return ret;
}

bool Function::hasIgnorableArguments() const
{
    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        if( m_arguments[i]->CanBeIgnored() )
        {
            return true;
        }
    }

    return false;
}

bool Function::hasSkippedArguments() const
{
    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        if( m_arguments[i]->IsSkipped() )
        {
            return true;
        }
    }

    return false;
}
