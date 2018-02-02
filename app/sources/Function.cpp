#include "Function.hpp"

#include <stdexcept>
#include <sstream>
#include <string>

#include "ClangHelper.hpp"

/*===========================================================================
 *                              PRIVATE CLASSES
 *===========================================================================*/

class FunctionReturn
{
public:
    FunctionReturn( const Config &config, std::string &signature, std::string &body )
    : m_config( config ), m_signature( signature ), m_body( body ), m_pendingCallClosures( 0 ), m_override( NULL )
    {
    }

    void ProcessInitial( const std::string funcName, const CXType &returnType );
    void ProcessFinal( const CXType &returnType );

private:
    void ProcessInitialOverride();
    void ProcessInitialType( const CXType &returnType );
    void ProcessInitialTypePointer( const CXType & returnType );
    void ProcessInitialTypeRVReference( const CXType & returnType );
    void ProcessInitialTypeTypedef( const CXType & returnType );

    void ProcessFinalOverride();
    void ProcessFinalType( const CXType &returnType, bool inheritConst );
    void ProcessFinalTypePointer( const CXType &returnType );
    void ProcessFinalTypeRVReference( const CXType &returnType );
    void ProcessFinalTypeTypedef( const CXType &returnType, bool inheritConst );

    const Config &m_config;
    std::string &m_signature;
    std::string &m_body;
    unsigned int m_pendingCallClosures;
    const Config::OverrideSpec *m_override;
};

class FunctionArgument
{
public:
    FunctionArgument( const Config &config, std::string &signature, std::string &body )
    : m_config( config ), m_signature( signature ), m_body( body )
    {
    }

    void Process( const std::string funcName, const CXCursor &arg, int argNum );

private:
    void ProcessOverride( const Config::OverrideSpec *override, std::string &argExpr );
    void ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst, std::string &argExpr );
    void ProcessTypePointer( const CXType &argType, const CXType &origArgType, std::string &argExpr );
    void ProcessTypeRVReference( const CXType &argType, const CXType &origArgType, std::string &argExpr );
    void ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, std::string &argExpr );
    void ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee );

    const Config &m_config;
    std::string &m_signature;
    std::string &m_body;
};

//*************************************************************************************************
//
//                                        FUNCTION PROCESSING
//
//*************************************************************************************************

Function::Function( const CXCursor &cursor, const Config &config )
    : m_cursor( cursor ), m_config( config )
{
}

Function::~Function()
{
}

bool Function::IsMockable() const
{
    // If the function has a definition, it means that it's defined inline, and therefore it's not mockable
    bool hasNoDefinition = clang_Cursor_isNull( clang_getCursorDefinition( m_cursor ) );

    // Only consider canonical cursors to avoid generating several mocks for the same function if it's
    // declared more than once
    bool isCanonical = clang_equalCursors( m_cursor, clang_getCanonicalCursor( m_cursor ) );

    return isCanonical && hasNoDefinition;
}

std::string Function::GenerateMock() const
{
    return GenerateMock(false);
}

std::string Function::GenerateMock( bool isMethod ) const
{
    try
    {
        std::string signature;
        std::string body;
        FunctionReturn returnProc( m_config, signature, body );
        FunctionArgument argumentProc( m_config, signature, body );

        // Get function name
        std::string funcName = getQualifiedName( m_cursor );

        // Get & process function return type (initial stage)
        const CXType returnType = clang_getCursorResultType( m_cursor );
        returnProc.ProcessInitial( funcName, returnType );

        // Process function name
        signature += " " + funcName + "(";
        body += "mock().actualCall(\"" + funcName + "\")";

        // Process arguments
        int numArgs = clang_Cursor_getNumArguments( m_cursor );
        for( int i = 0; i < numArgs; i++ )
        {
            if( i > 0 )
            {
                signature += ", ";
            }

            const CXCursor arg = clang_Cursor_getArgument( m_cursor, i );
            argumentProc.Process( funcName, arg, i );
        }

        // Process function return type (final stage)
        returnProc.ProcessFinal( returnType );

        // Generate final mock from signature and body
        std::string mockCode = signature + ")";

        if( isMethod && clang_CXXMethod_isConst ( m_cursor ) )
        {
            mockCode += " const";
        }

        mockCode += "\n{\n    " + body + ";\n}\n";

        return mockCode;
    }
    // LCOV_EXCL_START
    catch( std::runtime_error &e )
    {
        return "/*\n"
               " * Function: " + clang_getTypeSpelling( clang_getCursorResultType(m_cursor) ) + " " + clang_getCursorDisplayName(m_cursor) + "\n"
               " * Mock generation error: " + e.what() + "\n"
               " */\n";
    }
    // LCOV_EXCL_STOP
}

//*************************************************************************************************
//
//                                        INITIAL RETURN PROCESSING
//
//*************************************************************************************************

void FunctionReturn::ProcessInitial( const std::string funcName, const CXType &returnType )
{
    m_pendingCallClosures = 0;
    m_override = NULL;

    m_signature += clang_getTypeSpelling( returnType );

    if( returnType.kind != CXType_Void )
    {
        m_body += "return ";

        std::string overrideKey = funcName + "@";
        m_override = m_config.GetOverride( overrideKey );

        if( m_override != NULL )
        {
            ProcessInitialOverride();
        }
        else
        {
            ProcessInitialType( returnType );
        }
    }
}

void FunctionReturn::ProcessInitialOverride()
{
    m_body += m_override->GetArgExprModFront();
}

void FunctionReturn::ProcessInitialType( const CXType &returnType )
{
    switch( returnType.kind )
    {
        case CXType_Void:
        case CXType_Bool:
        case CXType_Int:
        case CXType_UInt:
        case CXType_Long:
        case CXType_ULong:
        case CXType_Double:
            // Nothing to be done
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessInitialTypePointer( returnType );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            // Dereference and cast mock return pointer to proper pointer type
            m_body += "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( ";
            m_pendingCallClosures++;
            break;

        case CXType_Typedef:
            ProcessInitialTypeTypedef( returnType );
            break;

        case CXType_Enum:
        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Short:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Float:
        case CXType_WChar:
        case CXType_Char16:
        case CXType_Char32:
            m_body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
            m_pendingCallClosures++;
            break;

        case CXType_Elaborated:
            ProcessInitialType( clang_Type_getNamedType( returnType ) );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported return type " + clang_getTypeSpelling( returnType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( returnType.kind) + ")" );
// LCOV_EXCL_STOP
    }
}

void FunctionReturn::ProcessInitialTypePointer( const CXType &returnType )
{
    if( returnType.kind == CXType_LValueReference )
    {
        // Dereference mock return pointer
        m_body += "* ";
    }
    else if( returnType.kind == CXType_RValueReference )
    {
        // Dereference mock return pointer
        m_body += "std::move( * ";
        m_pendingCallClosures++;
    }

    const CXType pointeeType = clang_getPointeeType( returnType );
    if( pointeeType.kind != CXType_Void &&
        ! ( clang_isConstQualifiedType( pointeeType ) &&
            ( returnType.kind == CXType_Pointer ) &&
            ( pointeeType.kind == CXType_Char_S ) ) )
    {
        // Cast mock return pointer to proper pointer type
        m_body += "static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        m_pendingCallClosures++;
    }
}

void FunctionReturn::ProcessInitialTypeTypedef( const CXType &returnType )
{
    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Record ) ||
        ( underlyingType.kind == CXType_Unexposed ) )
    {
        // Dereference mock return pointer
        m_body += "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( ";
        m_pendingCallClosures++;
    }
    else if( ( underlyingType.kind == CXType_LValueReference ) ||
             ( underlyingType.kind == CXType_RValueReference ) )
    {
        // Cast and dereference casted mock return pointer
        const CXType pointeeType = clang_getPointeeType( underlyingType );
        m_body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( * static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        m_pendingCallClosures += 2;
    }
    else
    {
        // Just cast
        m_body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
        m_pendingCallClosures++;
    }
}

//*************************************************************************************************
//
//                                        FINAL RETURN PROCESSING
//
//*************************************************************************************************

void FunctionReturn::ProcessFinal( const CXType &returnType )
{
    if( m_override != NULL )
    {
        ProcessFinalOverride();
    }
    else
    {
        ProcessFinalType( returnType, false );
    }
}

void FunctionReturn::ProcessFinalOverride()
{
    m_body += ".return" + m_override->GetType() + "Value()" + m_override->GetArgExprModBack();
}


void FunctionReturn::ProcessFinalType( const CXType &returnType, bool inheritConst )
{
    switch( returnType.kind )
    {
        case CXType_Void:
            // Nothing to be done
            break;

        case CXType_Bool:
            m_body += ".returnBoolValue()";
            break;

        case CXType_Int:
        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Short:
        case CXType_WChar:
        case CXType_Enum:
            m_body += ".returnIntValue()";
            break;

        case CXType_UInt:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Char16:
            m_body += ".returnUnsignedIntValue()";
            break;

        case CXType_Long:
            m_body += ".returnLongIntValue()";
            break;

        case CXType_ULong:
        case CXType_Char32:
            m_body += ".returnUnsignedLongIntValue()";
            break;

        case CXType_Float:
        case CXType_Double:
            m_body += ".returnDoubleValue()";
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessFinalTypePointer( returnType );
            break;

        case CXType_Typedef:
            ProcessFinalTypeTypedef( returnType, inheritConst );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            m_body += ".returnConstPointerValue()";
            break;

        case CXType_Elaborated:
            ProcessFinalType( clang_Type_getNamedType( returnType ), inheritConst );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported return type " + clang_getTypeSpelling( returnType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( returnType.kind) + ")" );
// LCOV_EXCL_STOP
    }

    // Close static casts if necessary
    while( m_pendingCallClosures )
    {
        m_body += " )";
        m_pendingCallClosures--;
    }
}

void FunctionReturn::ProcessFinalTypePointer( const CXType &returnType )
{
    const CXType pointeeType = clang_getPointeeType( returnType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( ( returnType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        m_body += ".returnStringValue()";
    }
    else
    {
        // Resolve possible typedefs
        const CXType underlyingPointeeType = clang_getCanonicalType( pointeeType );
        bool isUnderlyingPointeeConst = clang_isConstQualifiedType( underlyingPointeeType );

        if( isPointeeConst || isUnderlyingPointeeConst )
        {
            m_body += ".returnConstPointerValue()";
        }
        else
        {
            m_body += ".returnPointerValue()";
        }
    }
}

void FunctionReturn::ProcessFinalTypeTypedef( const CXType &returnType, bool inheritConst )
{
    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Pointer ) ||
        ( underlyingType.kind == CXType_LValueReference ) ||
        ( underlyingType.kind == CXType_RValueReference ) )
    {
        ProcessFinalTypePointer( underlyingType );
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( returnType ) || inheritConst;

        ProcessFinalType( underlyingType, isTypedefConst );
    }
}

//*************************************************************************************************
//
//                                     ARGUMENT PROCESSING
//
//*************************************************************************************************

void FunctionArgument::Process( const std::string funcName, const CXCursor &arg, int argNum )
{
    // Get argument type
    const CXType argType = clang_getCursorType( arg );
    m_signature += clang_getTypeSpelling( argType );

    // Get argument name
    std::string argName = toString( clang_getCursorSpelling( arg ) );
    if( argName.empty() )
    {
        std::ostringstream argNameStream;
        argNameStream << "_unnamedArg" << argNum;
        argName = argNameStream.str();
    }
    m_signature += " " + argName;

    std::string argExpr = argName;

    std::string overrideKey = funcName + "#" + argName;
    const Config::OverrideSpec *override = m_config.GetOverride( overrideKey );
    if( override != NULL )
    {
        ProcessOverride( override, argExpr );
    }
    else
    {
        ProcessType( argType, argType, false, argExpr );
    }

    // Add arguments name and value
    m_body += "\"" + argName + "\", ";
    m_body += argExpr + ")";
}

void FunctionArgument::ProcessOverride( const Config::OverrideSpec *override, std::string &argExpr )
{
    m_body += ".with" + override->GetType() + "Parameter(";
    argExpr = override->GetArgExprModFront() + argExpr + override->GetArgExprModBack();
}

void FunctionArgument::ProcessType( const CXType &argType, const CXType &origArgType, bool inheritConst, std::string &argExpr )
{
    switch( argType.kind )
    {
        case CXType_Bool:
            m_body += ".withBoolParameter(";
            break;

        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Int:
        case CXType_Short:
        case CXType_WChar:
            m_body += ".withIntParameter(";
            break;

        case CXType_Enum:
            m_body += ".withIntParameter(";
            argExpr = "static_cast<int>(" + argExpr + ")";
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UInt:
        case CXType_UShort:
        case CXType_Char16:
            m_body += ".withUnsignedIntParameter(";
            break;

        case CXType_Long:
            m_body += ".withLongIntParameter(";
            break;

        case CXType_ULong:
        case CXType_Char32:
            m_body += ".withUnsignedLongIntParameter(";
            break;

        case CXType_Float:
        case CXType_Double:
            m_body += ".withDoubleParameter(";
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
            ProcessTypePointer( argType, origArgType,argExpr );
            break;

        case CXType_RValueReference:
            ProcessTypeRVReference( argType, origArgType, argExpr );
            break;

        case CXType_Typedef:
            ProcessTypeTypedef( argType, origArgType, inheritConst, argExpr );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            ProcessTypeRecord( argType, origArgType, inheritConst, false );
            argExpr.insert(0, "&");
            break;

        case CXType_Elaborated:
            ProcessType( clang_Type_getNamedType( argType ), origArgType, inheritConst,argExpr );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported parameter type " + clang_getTypeSpelling( argType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( argType.kind) + ")" );
            break;
// LCOV_EXCL_STOP
    }
}

void FunctionArgument::ProcessTypePointer( const CXType &argType, const CXType &origArgType, std::string &argExpr )
{
    const CXType pointeeType = clang_getPointeeType( argType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( argType.kind != CXType_Pointer )
    {
        argExpr.insert(0, "&");
    }

    if( ( argType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        m_body += ".withStringParameter(";
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
                    ProcessTypeRecord( underlyingPointeeType, origArgType, true, true );
                    break;

                default:
                    m_body += ".withConstPointerParameter(";
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
                    m_body += ".withPointerParameter(";
                    break;

                case CXType_Record:
                case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
                    ProcessTypeRecord( underlyingPointeeType, origArgType, false, true );
                    break;

                default:
                    m_body += ".withOutputParameter(";
                    break;
            }
        }
    }
}

void FunctionArgument::ProcessTypeRVReference( const CXType &argType, const CXType &origArgType, std::string &argExpr )
{
    CXType pointeeType = clang_getPointeeType( argType );

    // Remove sugar like "struct", etc.
    if( pointeeType.kind == CXType_Elaborated )
    {
        pointeeType = clang_Type_getNamedType( pointeeType );
    }

    if( ( pointeeType.kind == CXType_Record ) ||
        ( pointeeType.kind == CXType_Unexposed ) )
    {
        ProcessTypePointer( argType, origArgType, argExpr );
    }
    else
    {
        bool isPointeeConst = clang_isConstQualifiedType( pointeeType );
        ProcessType( pointeeType, origArgType, isPointeeConst, argExpr );
    }
}

void FunctionArgument::ProcessTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, std::string &argExpr )
{
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
            m_body += ".withStringParameter(";
        }
        else if( isPointeeConst )
        {
            m_body += ".withConstPointerParameter(";
        }
        else
        {
            m_body += ".withPointerParameter(";
        }

        if( underlyingType.kind != CXType_Pointer )
        {
            argExpr.insert(0, "&");
        }
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( argType ) || inheritConst;
        ProcessType( underlyingType, origArgType, isTypedefConst, argExpr );
    }
}

void FunctionArgument::ProcessTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee )
{
    bool isConst = clang_isConstQualifiedType( argType ) || inheritConst;
    if( !isPointee || isConst )
    {
        m_body += ".withParameterOfType(\"";
    }
    else
    {
        m_body += ".withOutputParameterOfType(\"";
    }
    m_body += getBareTypeSpelling( m_config.UseUnderlyingTypedefType() ? argType : origArgType ) + "\", ";
}
