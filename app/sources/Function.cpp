/**
 * @file
 * @brief      Implementation of Function class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

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
    virtual ~Return() noexcept {}

    void SetOriginalType( const std::string &type ) noexcept
    {
        m_originalType = type;
    }

    std::string GetMockSignature() const noexcept
    {
        return m_originalType;
    }

    virtual std::string GetMockBodyFront() const noexcept = 0;

    virtual std::string GetMockBodyBack() const noexcept = 0;

    virtual std::string GetExpectationSignature() const noexcept = 0;

    virtual std::string GetExpectationCallArgument() const noexcept = 0;

    virtual std::string GetExpectationBody() const noexcept = 0;

protected:
    std::string m_originalType;
};

class ReturnVoid : public Function::Return
{
public:
    virtual ~ReturnVoid() noexcept {};

    virtual std::string GetMockBodyFront() const noexcept override
    {
        return "";
    }

    virtual std::string GetMockBodyBack() const noexcept override
    {
        return "";
    }

    virtual std::string GetExpectationSignature() const noexcept override
    {
        return "";
    }

    virtual std::string GetExpectationCallArgument() const noexcept override
    {
        return "";
    }

    virtual std::string GetExpectationBody() const noexcept override
    {
        return "";
    }
};

class ReturnStandard : public Function::Return
{
public:
    ReturnStandard() noexcept : m_expectationArgByRef(false), m_expectationNeedsCast(false), m_isRVReference(false), m_expectationUseBaseType(false) {}
    virtual ~ReturnStandard() noexcept {};

    void MockRetExprPrepend( const std::string &expr ) noexcept
    {
        m_mockRetExprFront.insert( 0, expr );
    }

    void MockRetExprAppend( const std::string &expr ) noexcept
    {
        m_mockRetExprBack.append( expr );
    }

    void ExpectationRetExprPrepend( const std::string &expr ) noexcept
    {
        m_expectationRetExprFront.insert( 0, expr );
    }

    void ExpectationRetExprAppend( const std::string &expr ) noexcept
    {
        m_expectationRetExprBack.append( expr );
    }

    virtual std::string GetMockBodyFront() const noexcept override
    {
        return "return " + m_mockRetExprFront;
    }

    virtual std::string GetMockBodyBack() const noexcept override
    {
        return "." + GetMockCall() + "()" + m_mockRetExprBack;
    }

    virtual std::string GetExpectationCallArgument() const noexcept override
    {
        return RETURN_ARG_NAME;
    }

    virtual std::string GetExpectationSignature() const noexcept override
    {
        if( m_expectationUseBaseType )
        {
            return GetExpectationBaseType() + " " RETURN_ARG_NAME;
        }
        else
        {
            return GetSignatureType() + (m_expectationArgByRef ? " &" : " ") + RETURN_ARG_NAME;
        }
    }

    virtual std::string GetExpectationBody() const noexcept override
    {
        return INDENT EXPECTED_CALL_VAR_NAME ".andReturnValue(" + GetExpectationRetExprFront() +
               RETURN_ARG_NAME +  GetExpectationRetExprBack() + ");\n";
    }

    void SetExpectationArgByRef() noexcept
    {
        m_expectationArgByRef = true;
    }

    void SetExpectationNeedsCast() noexcept
    {
        m_expectationNeedsCast = true;
    }

    void SetIsRVReference() noexcept
    {
        m_isRVReference = true;
    }

    void SetExpectationUseBaseType() noexcept
    {
        m_expectationUseBaseType = true;
    }

protected:
    virtual std::string GetMockCall() const noexcept = 0;

    virtual std::string GetExpectationBaseType() const noexcept = 0;

private:
    std::string GetExpectationRetExprFront() const noexcept
    {
        return ( m_expectationNeedsCast ? "static_cast<" + GetExpectationBaseType() + ">(" : "" ) + m_expectationRetExprFront;
    }

    std::string GetExpectationRetExprBack() const noexcept
    {
        return m_expectationRetExprBack +  ( m_expectationNeedsCast ? ")" : "" );
    }

    std::string GetSignatureType() const noexcept
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
    bool m_expectationUseBaseType;
};

class ReturnBool : public ReturnStandard
{
public:
    virtual ~ReturnBool() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "bool";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnBoolValue";
    }
};

class ReturnInt : public ReturnStandard
{
public:
    virtual ~ReturnInt() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "int";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnIntValue";
    }
};

class ReturnUnsignedInt : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedInt() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "unsigned int";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnUnsignedIntValue";
    }
};

class ReturnLong : public ReturnStandard
{
public:
    virtual ~ReturnLong() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "long";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnLongIntValue";
    }
};

class ReturnUnsignedLong : public ReturnStandard
{
public:
    virtual ~ReturnUnsignedLong() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "unsigned long";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnUnsignedLongIntValue";
    }
};

class ReturnDouble : public ReturnStandard
{
public:
    virtual ~ReturnDouble() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "double";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnDoubleValue";
    }
};

class ReturnString : public ReturnStandard
{
public:
    virtual ~ReturnString() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const char*";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnStringValue";
    }
};

class ReturnConstPointer : public ReturnStandard
{
public:
    virtual ~ReturnConstPointer() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const void*";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnConstPointerValue";
    }
};

class ReturnPointer : public ReturnStandard
{
public:
    virtual ~ReturnPointer() noexcept {};

protected:
    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "void*";
    }

    virtual std::string GetMockCall() const noexcept override
    {
        return "returnPointerValue";
    }
};

class ReturnParser
{
public:
    ReturnParser( const Config &config ) noexcept
    : m_config( config )
    {}

    Function::Return* Process( const std::string funcName, const CXType &returnType );

private:
    Function::Return* ProcessOverride( const Config::OverrideSpec *override );
    ReturnStandard* ProcessType( const CXType &returnType, bool inheritConst, bool enableCast );
    ReturnStandard* ProcessTypePointer( const CXType & returnType, bool enableCast ) noexcept;
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
        const Config::OverrideSpec *override = m_config.GetTypeOverride( overrideKey );

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

    switch( overrideType ) // LCOV_EXCL_BR_LINE: Defensive
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

// LCOV_EXCL_START: Defensive
        default:
            throw std::runtime_error( "<INTERNAL ERROR> Unsupported mocked return type override" );
// LCOV_EXCL_STOP
    }

    ret->MockRetExprPrepend( override->GetExprModFront() );
    ret->MockRetExprAppend( override->GetExprModBack() );
    ret->SetExpectationUseBaseType();

    return ret;
}

ReturnStandard* ReturnParser::ProcessType( const CXType &returnType, bool inheritConst, bool enableCast )
{
    ReturnStandard *ret;

    bool mockNeedsCast = false;

    switch( returnType.kind ) // LCOV_EXCL_BR_LINE: Defensive
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
            ret = ProcessTypePointer( returnType, enableCast );
            break;

        case CXType_RValueReference:
            ret = ProcessTypePointer( returnType, enableCast );
            ret->SetIsRVReference();
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

// LCOV_EXCL_START: Defensive
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

ReturnStandard* ReturnParser::ProcessTypePointer( const CXType &returnType, bool enableCast ) noexcept
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
    virtual ~Argument() noexcept {}

    void SetName( const std::string &name ) noexcept
    {
        m_name = name;
    }

    void SetOriginalType( const std::string &type ) noexcept
    {
        m_originalType = type;
    }

    virtual std::string GetMockSignature() const noexcept = 0;

    virtual std::string GetMockBody() const noexcept = 0;

    virtual std::string GetExpectationSignature() const noexcept = 0;

    virtual std::string GetExpectationCallArgument() const noexcept = 0;

    virtual std::string GetExpectationBody( bool argumentsSkipped ) const noexcept = 0;

    virtual bool CanBeIgnored() const noexcept = 0;

    virtual bool IsSkipped() const noexcept = 0;

protected:
    std::string m_name;
    std::string m_originalType;
};

class ArgumentSkip : public Function::Argument
{
public:
    virtual ~ArgumentSkip() noexcept {}

    virtual std::string GetMockSignature() const noexcept override
    {
        return m_originalType;
    }

    virtual std::string GetExpectationSignature() const noexcept override
    {
        return "";
    }

    virtual std::string GetMockBody() const noexcept override
    {
        return "";
    }

    virtual std::string GetExpectationCallArgument() const noexcept override
    {
        return "";
    }

    virtual std::string GetExpectationBody( bool ) const noexcept override
    {
        return "";
    }

    virtual bool IsSkipped() const noexcept override
    {
        return true;
    }

    // LCOV_EXCL_START
    virtual bool CanBeIgnored() const noexcept override
    {
        return false;
    }
    // LCOV_EXCL_STOP
};

class ArgumentStandard : public Function::Argument
{
public:
    ArgumentStandard() noexcept : m_expectationArgByRef(false), m_forceNotIgnored(false), m_isRVReference(false), m_expectationUseBaseType(false) {}
    virtual ~ArgumentStandard() noexcept {}

    virtual std::string GetMockSignature() const noexcept override
    {
        return m_originalType + " " + m_name;
    }

    virtual std::string GetExpectationSignature() const noexcept override
    {
        if( CanBeIgnored() )
        {
            if( m_expectationUseBaseType )
            {
                return "CppUMockGen::Parameter<" + GetExpectationBaseType() + "> " + m_name;
            }
            else
            {
                return "CppUMockGen::Parameter<" + m_originalType + (m_expectationArgByRef ? "&> " : "> ") + m_name;
            }
        }
        else
        {
            if( m_expectationUseBaseType )
            {
                return GetExpectationBaseType() + " " + m_name;
            }
            else
            {
                return GetSignatureType() + (m_expectationArgByRef ? " &" : " ") + m_name;
            }
        }
    }

    virtual std::string GetMockBody() const noexcept override
    {
        return "." + GetMockBodyCall();
    }

    virtual std::string GetExpectationCallArgument() const noexcept override
    {
        return m_name;
    }

    virtual std::string GetExpectationBody( bool argumentsSkipped ) const noexcept override
    {
        if( CanBeIgnored() )
        {
            if( argumentsSkipped )
            {
                return INDENT "if(!" + m_name + ".isIgnored()) { " EXPECTED_CALL_VAR_NAME "." + GetExpectationBodyCall( ".getValue()" ) + "; }\n";
            }
            else
            {
                return INDENT "if(" + m_name + ".isIgnored()) { " IGNORE_OTHERS_VAR_NAME " = true; } else { " EXPECTED_CALL_VAR_NAME "." +
                        GetExpectationBodyCall( ".getValue()" ) + "; }\n";
            }
        }
        else
        {
            return INDENT EXPECTED_CALL_VAR_NAME "." + GetExpectationBodyCall( "" ) + ";\n";
        }
    }

    virtual bool IsSkipped() const noexcept override
    {
        return false;
    }

    void MockArgExprPrepend( const std::string &expr ) noexcept
    {
        m_mockArgExprFront.insert( 0, expr );
    }

    void MockArgExprAppend( const std::string &expr ) noexcept
    {
        m_mockArgExprBack.append( expr );
    }

    void SetExpectationArgByRef() noexcept
    {
        m_expectationArgByRef = true;
    }

    virtual bool CanBeIgnored() const noexcept override
    {
        return !m_forceNotIgnored && isInput();
    }

    void ForceNotIgnored() noexcept
    {
        m_forceNotIgnored = true;
    }

    void SetIsRVReference() noexcept
    {
        m_isRVReference = true;
    }

    void SetExpectationUseBaseType() noexcept
    {
        m_expectationUseBaseType = true;
    }

protected:
    virtual bool isInput() const noexcept
    {
        return true;
    }

    virtual std::string GetCallFront(bool mock) const noexcept = 0;

    virtual std::string GetCallBack(bool) const noexcept
    {
        return ")";
    }

    virtual std::string GetExpectationBaseType() const noexcept = 0;

private:
    std::string GetMockBodyCall() const noexcept
    {
        return GetCallFront(true) + "\"" + m_name + "\", " + m_mockArgExprFront + m_name + m_mockArgExprBack + GetCallBack(true);
    }

    std::string GetExpectationBodyCall(const std::string& getter) const noexcept
    {
        return GetCallFront(false) + "\"" + m_name + "\", " + ( m_expectationUseBaseType ? "" : m_mockArgExprFront ) +
               m_name + getter + ( m_expectationUseBaseType ? "" : m_mockArgExprBack ) + GetCallBack(false);
    }

    std::string GetSignatureType() const noexcept
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
    bool m_expectationUseBaseType;
};

class ArgumentBool : public ArgumentStandard
{
public:
    virtual ~ArgumentBool() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withBoolParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "bool";
    }
};

class ArgumentInt : public ArgumentStandard
{
public:
    virtual ~ArgumentInt() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withIntParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "int";
    }
};

class ArgumentUnsignedInt : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedInt() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withUnsignedIntParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "unsigned int";
    }
};

class ArgumentLong : public ArgumentStandard
{
public:
    virtual ~ArgumentLong() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withLongIntParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "long";
    }
};

class ArgumentUnsignedLong : public ArgumentStandard
{
public:
    virtual ~ArgumentUnsignedLong() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withUnsignedLongIntParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "unsigned long";
    }
};

class ArgumentDouble : public ArgumentStandard
{
public:
    virtual ~ArgumentDouble() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withDoubleParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "double";
    }
};

class ArgumentString : public ArgumentStandard
{
public:
    virtual ~ArgumentString() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withStringParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const char*";
    }
};

class ArgumentPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentPointer() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withPointerParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "void*";
    }
};

class ArgumentConstPointer : public ArgumentStandard
{
public:
    virtual ~ArgumentConstPointer() noexcept {}

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withConstPointerParameter(";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const void*";
    }
};

class ArgumentOutput : public ArgumentStandard
{
public:
    ArgumentOutput() noexcept : m_calculateSizeFromType(false) {}
    virtual ~ArgumentOutput() noexcept {}

    virtual std::string GetMockSignature() const noexcept override
    {
        return m_originalType + " " + m_name;
    }

    virtual std::string GetExpectationSignature() const noexcept override
    {
        std::string ret = ArgumentStandard::GetExpectationSignature();
        if( !m_calculateSizeFromType )
        {
            ret += ", size_t " SIZEOF_VAR_PREFIX + m_name;
        }
        return ret;
    }

    virtual std::string GetExpectationCallArgument() const noexcept override
    {
        std::string ret = ArgumentStandard::GetExpectationCallArgument();
        if( !m_calculateSizeFromType )
        {
            ret += ", " SIZEOF_VAR_PREFIX + m_name;
        }
        return ret;
    }

    void CalculateSizeFromType() noexcept
    {
        m_calculateSizeFromType = true;
    }

protected:
    virtual bool isInput() const noexcept override
    {
        return false;
    }

    virtual std::string GetCallFront(bool mock) const noexcept override
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

    virtual std::string GetCallBack(bool mock) const noexcept
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

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const void*";
    }

    bool m_calculateSizeFromType;
};

class ArgumentInputOfType : public ArgumentStandard
{
public:
    virtual ~ArgumentInputOfType() noexcept {}

    void SetExposedType( const std::string &type ) noexcept
    {
        m_exposedType = type;
    }

    void SetExpectationArgType( const std::string &type ) noexcept
    {
        m_expectationArgType = type;
    }

protected:
    virtual std::string GetCallFront(bool) const noexcept override
    {
        return "withParameterOfType(\"" + m_exposedType + "\", ";
    }

    virtual std::string GetExpectationBaseType() const noexcept override
    {
        return "const " + m_expectationArgType + "*";
    }

    std::string m_exposedType;
    std::string m_expectationArgType;
};

class ArgumentOutputOfType : public ArgumentInputOfType
{
public:
    virtual ~ArgumentOutputOfType() noexcept {}

protected:
    virtual bool isInput() const noexcept override
    {
        return false;
    }

    virtual std::string GetCallFront(bool mock) const noexcept override
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
    ArgumentParser( const Config &config ) noexcept
    : m_config( config )
    {}

    Function::Argument* Process( const std::string funcName, const CXCursor &arg, int argNum );

private:
    Function::Argument* ProcessOverride( const Config::OverrideSpec *override );
    ArgumentStandard* ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst );
    ArgumentStandard* ProcessTypePointer( const CXType &argType, const CXType &origArgType ) noexcept;
    ArgumentStandard* ProcessTypeRVReference( const CXType &argType, const CXType &origArgType );
    ArgumentStandard* ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst );
    ArgumentStandard* ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee ) noexcept;

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
    const Config::OverrideSpec *override = m_config.GetTypeOverride( overrideKey );

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

    if( overrideType == MockedType::InputOfType )
    {
        ret = new ArgumentInputOfType;
        static_cast<ArgumentInputOfType*>(ret)->SetExposedType( override->GetExposedTypeName() );
        static_cast<ArgumentInputOfType*>(ret)->SetExpectationArgType( override->GetExpectationArgTypeName() );
    }
    else if( overrideType == MockedType::OutputOfType )
    {
        ret = new ArgumentOutputOfType;
        static_cast<ArgumentOutputOfType*>(ret)->SetExposedType( override->GetExposedTypeName() );
        static_cast<ArgumentOutputOfType*>(ret)->SetExpectationArgType( override->GetExpectationArgTypeName() );
    }
    else switch( overrideType ) // LCOV_EXCL_BR_LINE: Defensive
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

// LCOV_EXCL_START: Defensive
        default:
            throw std::runtime_error( "<INTERNAL ERROR> Unsupported mocked argument type override" );
// LCOV_EXCL_STOP
    }

    ret->MockArgExprPrepend( override->GetExprModFront() );
    ret->MockArgExprAppend( override->GetExprModBack() );
    ret->SetExpectationUseBaseType();

    return ret;
}

ArgumentStandard* ArgumentParser::ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst )
{
    ArgumentStandard *ret;

    switch( argType.kind ) // LCOV_EXCL_BR_LINE: Defensive
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

// LCOV_EXCL_START: Defensive
        default:
            throw std::runtime_error( "Unsupported parameter type " + clang_getTypeSpelling( argType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( argType.kind) + ")" );
            break;
// LCOV_EXCL_STOP
    }

    return ret;
}

ArgumentStandard* ArgumentParser::ProcessTypePointer( const CXType &argType, const CXType &origArgType ) noexcept
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
                    ret = new ArgumentPointer;
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

ArgumentStandard* ArgumentParser::ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee ) noexcept
{
    ArgumentInputOfType *ret;

    bool isConst = clang_isConstQualifiedType( argType ) || inheritConst;
    if( !isPointee || isConst )
    {
        ret = new ArgumentInputOfType;
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

Function::Function() noexcept
: m_isConstMethod( false ), m_isNonStaticMethod( false ), m_exceptionSpec( EExceptionSpec::Any )
{
}

// LCOV_EXCL_START
Function::~Function() noexcept
{
}
// LCOV_EXCL_STOP

bool Function::IsMockable( const CXCursor &cursor ) const noexcept
{
    // If the function has a definition, it means that it's defined inline, and therefore it's not mockable
    bool hasNoDefinition = clang_Cursor_isNull( clang_getCursorDefinition( cursor ) );

    // Only consider canonical cursors to avoid generating several mocks for the same function if it's
    // declared more than once
    bool isCanonical = clang_equalCursors( cursor, clang_getCanonicalCursor( cursor ) );

    return isCanonical && hasNoDefinition;
}

std::string Function::GetExpectationFunctionName( const std::string &functionName ) const noexcept
{
    return functionName;
}

static Function::EExceptionSpec ParseClangExceptionSpec( const CXCursor &cursor )
{
    int exceptionSpec = clang_getCursorExceptionSpecificationType( cursor );

    if( exceptionSpec < 0 )
    {
        return Function::EExceptionSpec::Any;
    }

    switch( exceptionSpec ) // LCOV_EXCL_BR_LINE: Defensive
    {
        case CXCursor_ExceptionSpecificationKind_None:
        case CXCursor_ExceptionSpecificationKind_Unevaluated:
            return Function::EExceptionSpec::Any; // No spec means any exception can be thrown

        case CXCursor_ExceptionSpecificationKind_DynamicNone:
            return Function::EExceptionSpec::DynamicNone;

        case CXCursor_ExceptionSpecificationKind_Dynamic:
            return Function::EExceptionSpec::Dynamic;

        case CXCursor_ExceptionSpecificationKind_MSAny:
            return Function::EExceptionSpec::MSAny;

        case CXCursor_ExceptionSpecificationKind_BasicNoexcept:
            return Function::EExceptionSpec::None;

        // LCOV_EXCL_START: Defensive
        default:
            throw std::runtime_error( "<UNEXPECTED ERROR> Unexpected exception kind '" + std::to_string(exceptionSpec)  +
                                      "' for method '" + clang_getCursorSpelling(cursor) + "'" );
        // LCOV_EXCL_STOP
    }
}

bool Function::Parse( const CXCursor &cursor, const Config &config )
{
    if( IsMockable(cursor) )
    {
        ReturnParser returnParser( config );
        ArgumentParser argumentParser( config );

        // Get function name
        m_functionName = getQualifiedName( cursor );

        auto cursorKind = clang_getCursorKind( cursor );

        bool isMethod = ( cursorKind == CXCursor_CXXMethod );
        bool isConstructor = ( cursorKind == CXCursor_Constructor );
        bool isDestructor = ( cursorKind == CXCursor_Destructor );

        if( isMethod || isConstructor || isDestructor )
        {
            // Get method class
            m_className = getMemberClassName(cursor);

            // LCOV_EXCL_START
            if( m_className.empty() )
            {
                throw std::runtime_error( "<UNEXPECTED ERROR> Could not determine class name for method '" + clang_getCursorSpelling(cursor) + "'" );
            }
            // LCOV_EXCL_STOP
        }

        if( isMethod )
        {
            // Get method staticness
            m_isNonStaticMethod = !clang_CXXMethod_isStatic( cursor );

            // Get method constantness
            m_isConstMethod = clang_CXXMethod_isConst(cursor);
        }

        if( isDestructor )
        {
            m_isNonStaticMethod = true;
        }

        // Get exception specification
        m_exceptionSpec = ParseClangExceptionSpec(cursor);

        if( !isConstructor && !isDestructor )
        {
            // Get & process function return type
            const CXType returnType = clang_getCursorResultType( cursor );
            m_return = std::unique_ptr<Return>( returnParser.Process( m_functionName, returnType ) );
        }

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

static std::string ExceptionSpecToString( Function::EExceptionSpec exceptionSpec ) noexcept
{
    switch(exceptionSpec)
    {
        case Function::EExceptionSpec::DynamicNone:
            return " throw()";

        case Function::EExceptionSpec::MSAny:
            return " throw(...)";

        case Function::EExceptionSpec::None:
            return " noexcept";

        case Function::EExceptionSpec::Dynamic:
            return " throw(__put_exception_types_manually_here__)";

        default:
            return "";
    }
}

std::string Function::GenerateMock() const noexcept
{
// LCOV_EXCL_START
    if( m_functionName.empty() )
    {
        return "";
    }
// LCOV_EXCL_STOP

    std::string signature;
    std::string body;

    if( m_return )
    {
        signature = m_return->GetMockSignature() + " ";
        body = m_return->GetMockBodyFront();
    }

    signature += m_functionName + "(";
    body += "mock().actualCall(\"" + m_functionName + "\")";

    if( m_isNonStaticMethod )
    {
        body += ".onObject(this)";
    }

    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        if( i > 0 )
        {
            signature += ", ";
        }

        signature += m_arguments[i]->GetMockSignature();

        body += m_arguments[i]->GetMockBody();
    }

    signature += ")";

    if( m_isConstMethod )
    {
        signature += " const";
    }

    signature += ExceptionSpecToString( m_exceptionSpec );

    if( m_return )
    {
        body += m_return->GetMockBodyBack();
    }

    return signature + "\n{\n    " + body + ";\n}\n";
}

static std::string namespaceSeparator = "::";

static std::vector<std::string> GetNamespaceDecomposition( const std::string &qualifiedName ) noexcept
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

std::string Function::GenerateExpectation( bool proto ) const noexcept
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

    std::string functionName = GetExpectationFunctionName( namespaces[namespaces.size()-1] );

    // Function processing
    ret += GenerateExpectation( proto, functionName, true );
    ret += GenerateExpectation( proto, functionName, false );

    // Namespace closing
    for( size_t i = 0; i < (namespaces.size() - 1); i++ )
    {
        ret += "} ";
    }
    ret += "}\n";

    return ret;
}

std::string Function::GenerateExpectation( bool proto, const std::string &functionName, bool oneCall ) const noexcept
{
    bool addSignatureSeparator = false;
    bool argumentsSkipped = HasSkippedArguments();
    bool checkIgnoredArguments = !argumentsSkipped && HasIgnorableArguments();

    // Generate function signature initial part (name, number of calls paramater, and object paramater)

    std::string ret = "MockExpectedCall& " + functionName + "(";

    if( !oneCall )
    {
        ret += "unsigned int " NUM_CALLS_ARG_NAME;
        addSignatureSeparator = true;
    }

    if( m_isNonStaticMethod )
    {
        if( addSignatureSeparator )
        {
            ret += ", ";
        }

        ret += "CppUMockGen::Parameter<const " + m_className + "*> " OBJECT_ARG_NAME;

        if( !m_return && proto )
        {
            // Destructor => Add default value
            ret += " = ::CppUMockGen::IgnoreParameter::YES";
        }

        addSignatureSeparator = true;
    }

    // Generate function body initial part (name, number of calls paramater, and object paramater)

    std::string body;
    if( !proto )
    {
        if( oneCall )
        {
            body += INDENT "return " + functionName + "(1";

            if( m_isNonStaticMethod )
            {
                body += ", " OBJECT_ARG_NAME;
            }
        }
        else
        {
            if( checkIgnoredArguments )
            {
                body += INDENT "bool " IGNORE_OTHERS_VAR_NAME " = false;\n";
            }

            body += INDENT "MockExpectedCall& " EXPECTED_CALL_VAR_NAME " = mock().expectNCalls(" NUM_CALLS_ARG_NAME ", \"" + m_functionName + "\");\n";

            if( m_isNonStaticMethod )
            {
                body += INDENT "if(!" OBJECT_ARG_NAME ".isIgnored()) { " EXPECTED_CALL_VAR_NAME ".onObject(const_cast<" +
                        m_className + "*>(" OBJECT_ARG_NAME ".getValue())); }\n";
            }
        }
    }

    // Generate function signature and body arguments part

    for( size_t i = 0; i < m_arguments.size(); i++ )
    {
        // Generate function signature argument part

        std::string argumentSignature = m_arguments[i]->GetExpectationSignature();

        if( !argumentSignature.empty() )
        {
            if( addSignatureSeparator )
            {
                ret += ", ";
            }

            ret += argumentSignature;
            addSignatureSeparator = true;
        }

        // Generate function body argument part

        if( !proto )
        {
            if( oneCall )
            {
                std::string callArgument = m_arguments[i]->GetExpectationCallArgument();
                if( !callArgument.empty() )
                {
                    body += ", " + callArgument;
                }
            }
            else
            {
                body += m_arguments[i]->GetExpectationBody( argumentsSkipped );
            }
        }
    }

    // Generate function signature and body return part

    if( m_return )
    {
        // Generate function signature return part

        std::string returnSignature = m_return->GetExpectationSignature();

        if( !returnSignature.empty() )
        {
            if( addSignatureSeparator )
            {
                ret += ", ";
            }

            ret += returnSignature;
        }

        // Generate function body return part

        if( !proto )
        {
            if( oneCall )
            {
                std::string returnArgument = m_return->GetExpectationCallArgument();
                if( !returnArgument.empty() )
                {
                    body += ", " + returnArgument;
                }
            }
            else
            {
                body += m_return->GetExpectationBody();
            }
        }
    }

    // Generate function signature and body final part

    if( proto )
    {
        ret += ");\n";
    }
    else
    {
        if( oneCall )
        {
            body += ");";
        }
        else
        {
            if( argumentsSkipped )
            {
                body += INDENT EXPECTED_CALL_VAR_NAME ".ignoreOtherParameters();\n";
            }
            else if( checkIgnoredArguments )
            {
                body += INDENT "if(" IGNORE_OTHERS_VAR_NAME ") { " EXPECTED_CALL_VAR_NAME ".ignoreOtherParameters(); }\n";
            }
            body += INDENT "return " EXPECTED_CALL_VAR_NAME ";";
        }

        ret += ")\n{\n" + body + "\n}\n";
    }

    return ret;
}

bool Function::HasIgnorableArguments() const noexcept
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

bool Function::HasSkippedArguments() const noexcept
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
