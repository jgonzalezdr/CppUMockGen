#include "Function.hpp"

#include <stdexcept>
#include <sstream>
#include "ClangHelper.hpp"

Function::Function( const CXCursor &cursor, const Config &config )
    : m_cursor( cursor ), m_config( config )
{}

bool Function::IsMockable() const
{
    // If the function has a definition, it means that it's defined inline, and therefore it's not mockable
    bool hasNoDefinition = clang_Cursor_isNull( clang_getCursorDefinition( m_cursor ) );

    // Only consider canonical cursors to avoid generating several mocks for the same function if it's
    // declared more than once
    bool isCanonical = clang_equalCursors( m_cursor, clang_getCanonicalCursor( m_cursor ) );

    // TODO: Print warning if not mockable

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
        Mock mock;

        // Get & process function return type (initial stage)
        const CXType returnType = clang_getCursorResultType( m_cursor );
        unsigned int pendingCallClosures = ProcessReturnInitial( returnType, mock );

        // Get & process function name
        std::string funcName = getQualifiedName( m_cursor );
        mock.signature += " " + funcName + "(";
        mock.body += "mock().actualCall(\"" + funcName + "\")";

        // Process arguments
        int numArgs = clang_Cursor_getNumArguments( m_cursor );
        for( int i = 0; i < numArgs; i++ )
        {
            if( i > 0 )
            {
                mock.signature += ", ";
            }

            const CXCursor arg = clang_Cursor_getArgument( m_cursor, i );
            ProcessArgument( arg, mock, i );
        }

        // Process function return type (final stage)
        ProcessReturnFinal( returnType, mock, false );

        // Close static casts if necessary
        while( pendingCallClosures )
        {
            mock.body += " )";
            pendingCallClosures--;
        }

        std::string mockCode = mock.signature + ")";

        if( isMethod && clang_CXXMethod_isConst ( m_cursor ) )
        {
            mockCode += " const";
        }

        mockCode += "\n{\n    " + mock.body + ";\n}\n";

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

unsigned int Function::ProcessReturnInitial( const CXType &returnType, Mock &mock ) const
{
    unsigned int pendingCallClosures = 0;

    mock.signature += clang_getTypeSpelling( returnType );

    if( returnType.kind != CXType_Void )
    {
        mock.body += "return ";

        pendingCallClosures = ProcessReturnInitialType( returnType, mock );
    }

    return pendingCallClosures;
}

unsigned int Function::ProcessReturnInitialType( const CXType &returnType, Mock &mock ) const
{
    unsigned int pendingCallClosures = 0;

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
            pendingCallClosures = ProcessReturnInitialTypePointer( returnType, mock );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            // Dereference and cast mock return pointer to proper pointer type
            mock.body += "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( ";
            pendingCallClosures++;
            break;

        case CXType_Typedef:
            pendingCallClosures = ProcessReturnInitialTypeTypedef( returnType, mock );
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
            mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
            pendingCallClosures++;
            break;

        case CXType_Elaborated:
            pendingCallClosures = ProcessReturnInitialType( clang_Type_getNamedType( returnType ), mock );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported return type " + clang_getTypeSpelling( returnType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( returnType.kind) + ")" );
// LCOV_EXCL_STOP
    }

    return pendingCallClosures;
}

unsigned int Function::ProcessReturnInitialTypePointer( const CXType &returnType, Mock &mock ) const
{
    unsigned int pendingCallClosures = 0;

    if( returnType.kind == CXType_LValueReference )
    {
        // Dereference mock return pointer
        mock.body += "* ";
    }
    else if( returnType.kind == CXType_RValueReference )
    {
        // Dereference mock return pointer
        mock.body += "std::move( * ";
        pendingCallClosures++;
    }

    const CXType pointeeType = clang_getPointeeType( returnType );
    if( pointeeType.kind != CXType_Void &&
        ! ( clang_isConstQualifiedType( pointeeType ) &&
            ( returnType.kind == CXType_Pointer ) &&
            ( pointeeType.kind == CXType_Char_S ) ) )
    {
        // Cast mock return pointer to proper pointer type
        mock.body += "static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        pendingCallClosures++;
    }

    return pendingCallClosures;
}

unsigned int Function::ProcessReturnInitialTypeTypedef( const CXType &returnType, Mock &mock ) const
{
    unsigned int pendingCallClosures = 1;
    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Record ) ||
        ( underlyingType.kind == CXType_Unexposed ) )
    {
        // Dereference mock return pointer
        mock.body += "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( ";
    }
    else if( ( underlyingType.kind == CXType_LValueReference ) ||
             ( underlyingType.kind == CXType_RValueReference ) )
    {
        // Cast and dereference casted mock return pointer
        const CXType pointeeType = clang_getPointeeType( underlyingType );
        mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( * static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        pendingCallClosures++;
    }
    else
    {
        // Just cast
        mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
    }

    return pendingCallClosures;
}

void Function::ProcessReturnFinal( const CXType &returnType, Mock &mock, bool inheritConst ) const
{
    switch( returnType.kind )
    {
        case CXType_Void:
            // Nothing to be done
            break;

        case CXType_Bool:
            mock.body += ".returnBoolValue()";
            break;

        case CXType_Int:
        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Short:
        case CXType_WChar:
        case CXType_Enum:
            mock.body += ".returnIntValue()";
            break;

        case CXType_UInt:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Char16:
        case CXType_Char32:
            mock.body += ".returnUnsignedIntValue()";
            break;

        case CXType_Long:
            mock.body += ".returnLongIntValue()";
            break;

        case CXType_ULong:
            mock.body += ".returnUnsignedLongIntValue()";
            break;

        case CXType_Float:
        case CXType_Double:
            mock.body += ".returnDoubleValue()";
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessReturnFinalTypePointer( returnType, mock );
            break;

        case CXType_Typedef:
            ProcessReturnFinalTypeTypedef( returnType, mock, inheritConst );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            mock.body += ".returnConstPointerValue()";
            break;

        case CXType_Elaborated:
            ProcessReturnFinal( clang_Type_getNamedType( returnType ), mock, inheritConst );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported return type " + clang_getTypeSpelling( returnType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( returnType.kind) + ")" );
// LCOV_EXCL_STOP
    }
}

void Function::ProcessReturnFinalTypePointer( const CXType &returnType, Mock &mock ) const
{
    const CXType pointeeType = clang_getPointeeType( returnType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( ( returnType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        mock.body += ".returnStringValue()";
    }
    else
    {
        // Resolve possible typedefs
        const CXType underlyingPointeeType = clang_getCanonicalType( pointeeType );
        bool isUnderlyingPointeeConst = clang_isConstQualifiedType( underlyingPointeeType );

        if( isPointeeConst || isUnderlyingPointeeConst )
        {
            mock.body += ".returnConstPointerValue()";
        }
        else
        {
            mock.body += ".returnPointerValue()";
        }
    }
}

void Function::ProcessReturnFinalTypeTypedef( const CXType &returnType, Mock &mock, bool inheritConst ) const
{
    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Pointer ) ||
        ( underlyingType.kind == CXType_LValueReference ) ||
        ( underlyingType.kind == CXType_RValueReference ) )
    {
        ProcessReturnFinalTypePointer( underlyingType, mock );
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( returnType ) || inheritConst;

        ProcessReturnFinal( underlyingType, mock, isTypedefConst );
    }
}

void Function::ProcessArgument( const CXCursor &arg, Mock &mock, int argNum ) const
{
    // Get argument type
    const CXType argType = clang_getCursorType( arg );
    mock.signature += clang_getTypeSpelling( argType );

    // Get argument name
    std::string argName = toString( clang_getCursorSpelling( arg ) );
    if( argName.empty() )
    {
        std::ostringstream argNameStream;
        argNameStream << "_unnamedArg" << argNum;
        argName = argNameStream.str();
    }
    mock.signature += " " + argName;

    std::string argExpr = argName;
    ProcessArgumentType( argType, argType, false, mock, argExpr );

    // Add arguments name and value
    mock.body += "\"" + argName + "\", ";
    mock.body += argExpr + ")";
}

void Function::ProcessArgumentType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const
{
    switch( argType.kind )
    {
        case CXType_Bool:
            mock.body += ".withBoolParameter(";
            break;

        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Int:
        case CXType_Short:
        case CXType_WChar:
            mock.body += ".withIntParameter(";
            break;

        case CXType_Enum:
            mock.body += ".withIntParameter(";
            argExpr = "static_cast<int>(" + argExpr + ")";
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UInt:
        case CXType_UShort:
        case CXType_Char16:
        case CXType_Char32:
            mock.body += ".withUnsignedIntParameter(";
            break;

        case CXType_Long:
            mock.body += ".withLongIntParameter(";
            break;

        case CXType_ULong:
            mock.body += ".withUnsignedLongIntParameter(";
            break;

        case CXType_Float:
        case CXType_Double:
            mock.body += ".withDoubleParameter(";
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
            ProcessArgumentTypePointer( argType, origArgType, mock, argExpr );
            break;

        case CXType_RValueReference:
            ProcessArgumentTypeRVReference( argType, origArgType, mock, argExpr );
            break;

        case CXType_Typedef:
            ProcessArgumentTypeTypedef( argType, origArgType, inheritConst, mock, argExpr );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            ProcessArgumentTypeRecord( argType, origArgType, inheritConst, false, mock );
            argExpr.insert(0, "&");
            break;

        case CXType_Elaborated:
            ProcessArgumentType( clang_Type_getNamedType( argType ), origArgType, inheritConst, mock, argExpr );
            break;

// LCOV_EXCL_START
        default:
            throw std::runtime_error( "Unsupported parameter type " + clang_getTypeSpelling( argType ) +
                                      " (kind: " +  clang_getTypeKindSpelling( argType.kind) + ")" );
            break;
// LCOV_EXCL_STOP
    }
}

void Function::ProcessArgumentTypePointer( const CXType &argType, const CXType &origArgType, Mock &mock, std::string &argExpr ) const
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
        mock.body += ".withStringParameter(";
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
                    ProcessArgumentTypeRecord( underlyingPointeeType, origArgType, true, true, mock );
                    break;

                default:
                    mock.body += ".withConstPointerParameter(";
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
                    mock.body += ".withPointerParameter(";
                    break;

                case CXType_Record:
                case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
                    ProcessArgumentTypeRecord( underlyingPointeeType, origArgType, false, true, mock );
                    break;

                default:
                    // TODO: Differentiate pointer from output using user comments
                    mock.body += ".withOutputParameter(";
                    break;
            }
        }
    }
}

void Function::ProcessArgumentTypeRVReference( const CXType &argType, const CXType &origArgType, Mock &mock, std::string &argExpr ) const
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
        ProcessArgumentTypePointer( argType, origArgType, mock, argExpr );
        //throw std::runtime_error( "Unsupported parameter type " + clang_getTypeSpelling( argType ) );
    }
    else
    {
        bool isPointeeConst = clang_isConstQualifiedType( pointeeType );
        ProcessArgumentType( pointeeType, origArgType, isPointeeConst, mock, argExpr );
    }
}

void Function::ProcessArgumentTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock, std::string &argExpr ) const
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
            mock.body += ".withStringParameter(";
        }
        else if( isPointeeConst )
        {
            mock.body += ".withConstPointerParameter(";
        }
        else
        {
            mock.body += ".withPointerParameter(";
        }

        if( underlyingType.kind != CXType_Pointer )
        {
            argExpr.insert(0, "&");
        }
    }
    else
    {
        bool isTypedefConst = clang_isConstQualifiedType( argType ) || inheritConst;
        ProcessArgumentType( underlyingType, origArgType, isTypedefConst, mock, argExpr );
    }
}

void Function::ProcessArgumentTypeRecord( const CXType &argType, const CXType &origArgType, bool inheritConst, bool isPointee, Mock &mock ) const
{
    bool isConst = clang_isConstQualifiedType( argType ) || inheritConst;
    if( !isPointee || isConst )
    {
        mock.body += ".withParameterOfType(\"";
    }
    else
    {
        mock.body += ".withOutputParameterOfType(\"";
    }
    mock.body += getBareTypeSpelling( m_config.UseUnderlyingTypedefType() ? argType : origArgType ) + "\", ";
}
