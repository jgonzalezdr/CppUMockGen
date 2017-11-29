#include "Function.hpp"

#include <iostream>
#include "ClangHelper.hpp"

Function::Function( const CXCursor &cursor, const Config &config )
    : m_cursor( cursor ), m_config( config )
{
#if 0
    std::string name = getQualifiedName( cursor );

    std::cout << "Function:" << std::endl;
    std::cout << "  - Name = " << name << std::endl;
    std::cout << "  - Return Type = " << clang_getTypeSpelling( clang_getCursorResultType( cursor ) ) << std::endl;
    std::cout << "  - Return TypeKind = " << clang_getTypeKindSpelling( clang_getCursorResultType( cursor ).kind ) << std::endl;
    std::cout << "  - Is Definition = " << ( clang_isCursorDefinition( cursor ) ? "YES" : "NO" ) << std::endl;
    std::cout << "  - Has Definition = " << ( !clang_Cursor_isNull( clang_getCursorDefinition( cursor ) ) ? "YES" : "NO" ) << std::endl;
    std::cout << "  - Is Canonical = " << ( clang_equalCursors( cursor, clang_getCanonicalCursor(cursor) ) ? "YES" : "NO" ) << std::endl;
    std::cout << "  - Is Inline = " << ( clang_Cursor_isFunctionInlined( cursor ) ? "YES" : "NO" ) << std::endl;

    int numArgs = clang_Cursor_getNumArguments( cursor );
    for( int i = 0; i < numArgs; i++ )
    {
        const CXCursor arg = clang_Cursor_getArgument( cursor, i );

        std::cout << "  - Param " << (i + 1) << ":" << std::endl;
        std::cout << "    - Name = " << clang_getCursorSpelling( arg ) << std::endl;
        CXType argType = clang_getCursorType( arg );
        std::cout << "    - Type = " << clang_getTypeSpelling( argType ) << std::endl;
        //std::cout << "    - Type = " << clang_getTypeSpelling( clang_getArgType( clang_getCursorType( cursor ), i ) ) << std::endl;
        std::cout << "    - TypeKind = " << clang_getTypeKindSpelling( argType.kind ) << std::endl;
        std::cout << "    - Const = " << (clang_isConstQualifiedType( argType ) ? "YES" : "NO") << std::endl;
        if( ( argType.kind == CXType_Pointer ) || ( argType.kind == CXType_LValueReference )  || ( argType.kind == CXType_RValueReference ) )
        {
            CXType pointeeType = clang_getPointeeType( argType );

            std::cout << "      - Pointer Type = " << clang_getTypeSpelling( pointeeType ) << std::endl;
            std::cout << "      - Pointer TypeKind = " << clang_getTypeKindSpelling( pointeeType.kind ) << std::endl;
            std::cout << "      - Pointer Const = " << (clang_isConstQualifiedType( pointeeType ) ? "YES" : "NO") << std::endl;
        }
    }
#endif
}

Function::~Function()
{}

bool Function::IsMockable() const
{
    // If the function has a definition, it means that it's defined inline, and therefore it's not mockable
    bool hasNoDefinition = clang_Cursor_isNull( clang_getCursorDefinition( m_cursor ) );

    // Only consider canonical cursors to avoid generating several mocks for the same function if it's
    // declared more than once
    bool isCanonical = clang_equalCursors( m_cursor, clang_getCanonicalCursor( m_cursor ) );

    // TODO: Check return type and print warning if not mockable

    return isCanonical && hasNoDefinition;
}

std::string Function::GenerateMock() const
{
    Mock mock;

    // Get & process function return type (initial stage)
    const CXType returnType = clang_getCursorResultType( m_cursor );
    unsigned int castsPerformed = ProcessReturnTypeInitial( returnType, mock );

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
        ProcessArgument( arg, mock );
    }

    // Process function return type (final stage)
    ProcessReturnTypeFinal( returnType, mock, false, castsPerformed );

    return mock.signature + ")\n{\n    " + mock.body + ";\n}\n";
}

unsigned int Function::ProcessReturnTypeInitial( const CXType &returnType, Mock &mock ) const
{
    unsigned int castsPerformed = 0;

    mock.signature += clang_getTypeSpelling( returnType );

    if( returnType.kind != CXType_Void )
    {
        mock.body += "return ";
    }

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
            castsPerformed = ProcessReturnTypeInitialPointer( returnType, mock );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            // Dereference and cast mock return pointer to proper pointer type
            mock.body += "* static_cast<const " + clang_getTypeSpelling( returnType ) + " *>( ";
            castsPerformed++;
            break;

        case CXType_Typedef:
            castsPerformed = ProcessReturnTypeInitialTypedef( returnType, mock );
            break;

        case CXType_Enum:
        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Char16:
        case CXType_Short:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
        case CXType_Float:
            mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
            castsPerformed++;
            break;

// LCOV_EXCL_START
        default:
            std::cerr << "ERROR: Unexpected return type kind " << clang_getTypeKindSpelling( returnType.kind ) << std::endl;
            break;
// LCOV_EXCL_STOP
    }

    return castsPerformed;
}

unsigned int Function::ProcessReturnTypeInitialPointer( const CXType &returnType, Mock &mock ) const
{
    unsigned int castsPerformed = 0;

    if( ( returnType.kind == CXType_LValueReference ) ||
        ( returnType.kind == CXType_RValueReference ) )
    {
        // Dereference mock return pointer
        mock.body += "* ";
    }

    const CXType pointeeType = clang_getPointeeType( returnType );
    if( pointeeType.kind != CXType_Void &&
        ! ( clang_isConstQualifiedType( pointeeType ) &&
            ( returnType.kind == CXType_Pointer ) &&
            ( pointeeType.kind == CXType_Char_S ) ) )
    {
        // Cast mock return pointer to proper pointer type
        mock.body += "static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        castsPerformed++;
    }

    return castsPerformed;
}

unsigned int Function::ProcessReturnTypeInitialTypedef( const CXType &returnType, Mock &mock ) const
{
    unsigned int castsPerformed = 1;
    const CXType underlyingType = clang_getCanonicalType( returnType );

    if( ( underlyingType.kind == CXType_Record ) ||
        ( underlyingType.kind == CXType_Unexposed ) )
    {
        // Dereference mock return pointer
        mock.body += "* static_cast<" + clang_getTypeSpelling( returnType ) + " *>( ";
    }
    else if( ( underlyingType.kind == CXType_LValueReference ) ||
             ( underlyingType.kind == CXType_RValueReference ) )
    {
        // Cast and dereference casted mock return pointer
        const CXType pointeeType = clang_getPointeeType( underlyingType );
        mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( * static_cast<" + clang_getTypeSpelling( pointeeType ) + " *>( ";
        castsPerformed++;
    }
    else
    {
        // Just cast
        mock.body += "static_cast<" + clang_getTypeSpelling( returnType ) + ">( ";
    }

    return castsPerformed;
}

void Function::ProcessReturnTypeFinal( const CXType &returnType, Mock &mock, bool inheritConst, unsigned int castsPerformed ) const
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
        case CXType_Char16:
        case CXType_Char32:
        case CXType_Short:
        case CXType_Enum:
            mock.body += ".returnIntValue()";
            break;

        case CXType_UInt:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UShort:
            mock.body += ".returnUnsignedIntValue()";
            break;

        case CXType_Long:
            mock.body += ".returnLongValue()";
            break;

        case CXType_ULong:
            mock.body += ".returnUnsignedLongValue()";
            break;

        case CXType_Float:
        case CXType_Double:
            mock.body += ".returnDoubleValue()";
            break;

        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessReturnTypeFinalPointer( returnType, mock, inheritConst );
            break;

        case CXType_Typedef:
            ProcessReturnTypeFinalTypedef( returnType, mock, inheritConst );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            mock.body += ".returnConstPointerValue()";
            break;

// LCOV_EXCL_START
        default:
            std::cerr << "ERROR: Unexpected return type kind " << clang_getTypeKindSpelling( returnType.kind ) << std::endl;
            break;
// LCOV_EXCL_STOP
    }

    // Close static casts if necessary
    while( castsPerformed )
    {
        mock.body += " )";
        castsPerformed--;
    }
}

void Function::ProcessReturnTypeFinalPointer( const CXType &returnType, Mock &mock, bool inheritConst ) const
{
    const CXType pointeeType = clang_getPointeeType( returnType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    if( ( returnType.kind == CXType_Pointer ) &&
        ( pointeeType.kind == CXType_Char_S ) &&
        isPointeeConst )
    {
        mock.body += ".returnStringValue()";
    }
    else if( isPointeeConst || inheritConst )
    {
        mock.body += ".returnConstPointerValue()";
    }
    else
    {
        mock.body += ".returnPointerValue()";
    }
}

void Function::ProcessReturnTypeFinalTypedef( const CXType &returnType, Mock &mock, bool inheritConst ) const
{
    const CXType underlyingType = clang_getCanonicalType( returnType );
    bool isTypedefConst = clang_isConstQualifiedType( returnType ) || inheritConst;

    if( ( underlyingType.kind == CXType_Pointer ) ||
        ( underlyingType.kind == CXType_LValueReference ) ||
        ( underlyingType.kind == CXType_RValueReference ) )
    {
        ProcessReturnTypeFinalPointer( underlyingType, mock, isTypedefConst );
    }
    else
    {
        ProcessReturnTypeFinal( underlyingType, mock, isTypedefConst, 0 );
    }
}

void Function::ProcessArgument( const CXCursor &arg, Mock &mock ) const
{
    // Get argument type
    const CXType argType = clang_getCursorType( arg );
    mock.signature += clang_getTypeSpelling( argType );

    // Get argument name
    std::string argName = toString( clang_getCursorSpelling( arg ) );
    mock.signature += " " + argName;

    bool passArgumentAddress = ProcessArgumentType( argType, argType, false, mock );

    // Add arguments name and value
    mock.body += "\"" + argName + "\", ";
    if( passArgumentAddress )
    {
        mock.body += "&";
    }
    mock.body += argName + ")";
}

bool Function::ProcessArgumentType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const
{
    bool passArgumentAddress = false;

    switch( argType.kind )
    {
        case CXType_Bool:
            mock.body += ".withBoolParameter(";
            break;

        case CXType_Char_S:
        case CXType_SChar:
        case CXType_Char16:
        case CXType_Char32:
        case CXType_Int:
        case CXType_Short:
        case CXType_Enum:
            mock.body += ".withIntParameter(";
            break;

        case CXType_Char_U:
        case CXType_UChar:
        case CXType_UInt:
        case CXType_UShort:
            mock.body += ".withUnsignedIntParameter(";
            break;

        case CXType_Long:
            mock.body += ".withLongParameter(";
            break;

        case CXType_ULong:
            mock.body += ".withUnsignedLongParameter(";
            break;

        case CXType_Float:
        case CXType_Double:
            mock.body += ".withDoubleParameter(";
            break;

        case CXType_Pointer:
            ProcessArgumentTypePointer( argType, origArgType, inheritConst, mock );
            break;

        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessArgumentTypePointer( argType, origArgType, inheritConst, mock );
            passArgumentAddress = true;
            break;

        case CXType_Typedef:
            passArgumentAddress = ProcessArgumentTypeTypedef( argType, origArgType, inheritConst, mock );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            ProcessArgumentTypeRecord( argType, origArgType, inheritConst, false, mock );
            passArgumentAddress = true;
            break;

// LCOV_EXCL_START
        default:
            std::cerr << "ERROR: Unexpected parameter type kind " << clang_getTypeKindSpelling( argType.kind ) << std::endl;
            break;
// LCOV_EXCL_STOP
    }

    return passArgumentAddress;
}

void Function::ProcessArgumentTypePointer( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const
{
    const CXType pointeeType = clang_getPointeeType( argType );
    bool isPointeeConst = clang_isConstQualifiedType( pointeeType );

    // Resolve possible typedefs
    const CXType underlyingPointeeType = clang_getCanonicalType( pointeeType );
    bool isUnderlyingPointeeConst = clang_isConstQualifiedType( underlyingPointeeType );

    if( isPointeeConst || isUnderlyingPointeeConst || inheritConst )
    {
        switch( underlyingPointeeType.kind )
        {
            case CXType_Char_S:
                if( isUnderlyingPointeeConst )
                {
                    mock.body += ".withStringParameter(";
                }
                else
                {
                    mock.body += ".withConstPointerParameter(";
                }
                break;

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

bool Function::ProcessArgumentTypeTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const
{
    bool passArgumentAddress = false;
    const CXType underlyingType = clang_getCanonicalType( argType );
    bool isTypedefConst = clang_isConstQualifiedType( argType ) || inheritConst;

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
        else if( isPointeeConst || isTypedefConst )
        {
            mock.body += ".withConstPointerParameter(";
        }
        else
        {
            mock.body += ".withPointerParameter(";
        }

        passArgumentAddress = !( underlyingType.kind == CXType_Pointer );
    }
    else
    {
        passArgumentAddress = ProcessArgumentType( underlyingType, origArgType, isTypedefConst, mock );
    }

    return passArgumentAddress;
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

void Function::ProcessArgumentTypePointedTypedef( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const
{
    const CXType underlyingType = clang_getCanonicalType( argType );
    bool isTypedefConst = clang_isConstQualifiedType( argType ) || inheritConst;

    ProcessArgumentTypePointedType( underlyingType, origArgType, isTypedefConst, mock );
}

void Function::ProcessArgumentTypePointedType( const CXType &argType, const CXType &origArgType, bool inheritConst, Mock &mock ) const
{
    bool isTypeConst = clang_isConstQualifiedType( argType ) || inheritConst;

    switch( argType.kind )
    {
        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            ProcessArgumentTypePointedType( clang_getPointeeType( argType ), origArgType, isTypeConst, mock );
            break;

        case CXType_Typedef:
            ProcessArgumentTypePointedTypedef( argType, origArgType, isTypeConst, mock );
            break;

        case CXType_Record:
        case CXType_Unexposed: // Template classes are processed as "Unexposed" kind
            ProcessArgumentTypeRecord( argType, origArgType, isTypeConst, true, mock );
            break;

        default:
            if( isTypeConst )
            {
                mock.body += ".withConstPointerParameter(";
            }
            else
            {
                mock.body += ".withPointerParameter(";
            }
            break;
    }
}
