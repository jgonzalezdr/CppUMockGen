/**
 * @file
 * @brief      Implementation of Config class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "Config.hpp"

#include <set>
#include <vector>
#include <stdexcept>

#include "StringHelper.hpp"

static const char EXPR_MOD_SEPARATOR = '~';
static const char EXPR_CURRENT_ARG_PLACEHOLDER = '$';
static const char EXPECTATION_ARG_TYPE_SEPARATOR = '<';

Config::Config( bool interpretAsCpp, const std::string &languageStandard, bool useUnderlyingTypedefType,
                const std::vector<std::string> &typeOverrideOptions )
: m_interpretAsCpp( interpretAsCpp ), m_languageStandard( languageStandard ), m_useUnderlyingTypedefType( useUnderlyingTypedefType ),
  m_typeOverrideMap( typeOverrideOptions )
{
}

bool Config::InterpretAsCpp() const noexcept
{
    return m_interpretAsCpp;
}

const std::string& Config::GetLanguageStandard() const noexcept
{
    return m_languageStandard;
}

bool Config::UseUnderlyingTypedefType() const noexcept
{
    return m_useUnderlyingTypedefType;
}

const Config::OverrideSpec* Config::GetTypeOverride( const std::string& key ) const noexcept
{
    return m_typeOverrideMap.GetOverride(key);
}

static const std::vector<std::pair<std::string, MockedType>> validOverrideTypes =
{
    { "Bool", MockedType::Bool },
    { "Int", MockedType::Int },
    { "UnsignedInt", MockedType::UnsignedInt },
    { "LongInt", MockedType::Long },
    { "UnsignedLongInt", MockedType::UnsignedLong },
    { "Double", MockedType::Double },
    { "String", MockedType::String },
    { "Pointer", MockedType::Pointer },
    { "ConstPointer", MockedType::ConstPointer },
    { "Output", MockedType::Output },
    { "InputPOD", MockedType::InputPOD },
    { "OutputPOD", MockedType::OutputPOD },
    { "Skip", MockedType::Skip },
};

static const std::map<std::string, MockedType> validReturnOverrideTypes( validOverrideTypes.begin(), validOverrideTypes.end() - 4 );
static const std::map<std::string, MockedType> validParameterOverrideTypes( validOverrideTypes.begin(), validOverrideTypes.end() );

static const std::vector<std::pair<std::string, MockedType>> validExtendedOverrideTypes =
{
    { "InputOfType:", MockedType::InputOfType },
    { "OutputOfType:", MockedType::OutputOfType },
    { "MemoryBuffer:", MockedType::MemoryBuffer },
};

static bool ParseExpression( const std::string &expression, std::string &front, std::string &back )
{
    size_t placeholderPos = expression.find( EXPR_CURRENT_ARG_PLACEHOLDER );
    if( placeholderPos == std::string::npos )
    {
        front = expression;
        back.clear();
        return false;
    }
    else
    {
        front = expression.substr( 0, placeholderPos );
        back = expression.substr( placeholderPos + 1 );
        return true;
    }
}

class OptionError : public std::runtime_error
{
public:
    OptionError( const std::string &msg ) : runtime_error( msg ) {}
};

Config::OverrideSpec::OverrideSpec( const std::string &value, bool isReturn )
{
    m_type = MockedType::Skip;

    if( value.empty() )
    {
        throw OptionError( "Override option specification cannot be empty" );
    }

    std::string type;
    size_t exprModSepPos = value.find( EXPR_MOD_SEPARATOR );
    if( exprModSepPos != std::string::npos )
    {
        type = TrimString( value.substr(0, exprModSepPos) );
        if( type.empty() )
        {
            throw OptionError( "Override option type cannot be empty" );
        }

        std::string argExprMod = TrimString( value.substr(exprModSepPos+1) );
        if( argExprMod.empty() )
        {
            throw OptionError( "Override option argument expression cannot be empty if specified" );
        }

        if( !ParseExpression( argExprMod, m_exprModFront, m_exprModBack ) )
        {
            throw OptionError( "Override option argument expression does not contain parameter name placeholder ($)" );
        }
    }
    else
    {
        type = value;
    }

    if( isReturn )
    {
        auto it = validReturnOverrideTypes.find( type );
        if( it != validReturnOverrideTypes.end() )
        {
            m_type = it->second;
        }
        else
        {
            throw OptionError( "Invalid return override option type" );
        }
    }
    else
    {
        auto it = validParameterOverrideTypes.find( type );
        if( it != validParameterOverrideTypes.end() )
        {
            m_type = it->second;
        }
        else
        {
            bool overrideTypeFound = false;

            for( auto overrideType : validExtendedOverrideTypes )
            {
                if( type.find( overrideType.first ) == 0 )
                {
                    m_type = overrideType.second;
                    if( m_type == MockedType::MemoryBuffer )
                    {
                        auto sizeExpr = TrimString( type.substr( overrideType.first.size() ) );

                        if( sizeExpr.empty() )
                        {
                            throw OptionError( "Override option memory buffer size expression cannot be empty" );
                        }

                        m_hasSizeExprPlaceholder = ParseExpression( sizeExpr, m_sizeExprFront, m_sizeExprBack );
                    }
                    else
                    {
                        m_exposedTypeName = TrimString( type.substr( overrideType.first.size() ) );

                        size_t expectArgTypeSepPos = m_exposedTypeName.find( EXPECTATION_ARG_TYPE_SEPARATOR );
                        if( expectArgTypeSepPos != std::string::npos )
                        {
                            m_expectationArgTypeName = TrimString( m_exposedTypeName.substr( expectArgTypeSepPos + 1 ) );
                            m_exposedTypeName = TrimString( m_exposedTypeName.substr( 0, expectArgTypeSepPos ) );

                            if( m_expectationArgTypeName.empty() )
                            {
                                throw OptionError( "Override option expectation argument type cannot be empty" );
                            }
                        }
                        else
                        {
                            m_expectationArgTypeName = m_exposedTypeName;
                        }

                        if( m_exposedTypeName.empty() )
                        {
                            throw OptionError( "Override option type name cannot be empty" );
                        }
                    }

                    overrideTypeFound = true;
                }
            }

            if( !overrideTypeFound )
            {
                throw OptionError( "Invalid parameter override option type" );
            }
        }
    }
}

Config::OverrideMap::OverrideMap( const std::vector<std::string> &options )
{
    for( const std::string &option : options )
    {
        size_t sepPos = option.find('=');
        if( sepPos != std::string::npos )
        {
            bool isReturn = false;
            std::string key = TrimString( option.substr(0, sepPos) );
            if( key.empty() )
            {
                std::string errorMsg = "Override option key cannot be empty ['" + option + "'].";
                throw std::runtime_error( errorMsg );
            }
            else if( StringCount( key, '#' ) == 1 )
            {
                isReturn = false;
            }
            else if( ( StringCount( key, '@' ) == 1 ) &&
                     ( StringCount( key, '#' ) == 0 ) &&
                     ( ( key.front() == '@' ) || ( key.back() == '@' ) ) )
            {
                isReturn = true;
            }
            else
            {
                std::string errorMsg = "Invalid override option key format ['" + option + "'].";
                throw std::runtime_error( errorMsg );
            }

            try
            {
                Config::OverrideSpec spec = Config::OverrideSpec( option.substr(sepPos+1), isReturn );

                if( !m_map.emplace( key, spec ).second )
                {
                    std::string errorMsg = "Override option key '" + key + "' can only be passed once.";
                    throw std::runtime_error( errorMsg );
                }
            }
            catch( const OptionError& e )
            {
                std::string errorMsg = e.what();
                errorMsg += " ['" + option + "'].";
                throw std::runtime_error( errorMsg );
            }
        }
        else
        {
            std::string errorMsg = "Invalid override option '[" + option + "'].";
            throw std::runtime_error( errorMsg );
        }
    }
}

const Config::OverrideSpec* Config::OverrideMap::GetOverride( const std::string& key ) const noexcept
{
    OverrideMapType::const_iterator it = m_map.find( key );
    return ( it != m_map.end() ) ? &(it->second) : NULL;
}

MockedType Config::OverrideSpec::GetType() const noexcept
{
    return m_type;
}

const std::string& Config::OverrideSpec::GetExposedTypeName() const noexcept
{
    return m_exposedTypeName;
}

const std::string& Config::OverrideSpec::GetExprModFront() const noexcept
{
    return m_exprModFront;
}

const std::string& Config::OverrideSpec::GetExprModBack() const noexcept
{
    return m_exprModBack;
}

const bool Config::OverrideSpec::HasSizeExprPlaceholder() const noexcept
{
    return m_hasSizeExprPlaceholder;
}

const std::string& Config::OverrideSpec::GetSizeExprFront() const noexcept
{
    return m_sizeExprFront;
}

const std::string& Config::OverrideSpec::GetSizeExprBack() const noexcept
{
    return m_sizeExprBack;
}

const std::string& Config::OverrideSpec::GetExpectationArgTypeName() const noexcept
{
    return m_expectationArgTypeName;
}
