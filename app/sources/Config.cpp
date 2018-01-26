#include "Config.hpp"

#include <set>

Config::Config( bool useUnderlyingTypedefType, const std::vector<std::string> &overrideOptions )
: m_useUnderlyingTypedefType( useUnderlyingTypedefType ), m_overrideMap( overrideOptions )
{};

bool Config::UseUnderlyingTypedefType() const
{
    return m_useUnderlyingTypedefType;
}

const Config::OverrideSpec* Config::GetOverride( const std::string& key ) const
{
    return m_overrideMap.GetOverride(key);
}

static const std::set<std::string> validOverrideTypes =
{
    "Bool",
    "Int",
    "UnsignedInt",
    "Long",
    "UnsignedLong",
    "Double",
    "String",
    "Pointer",
    "ConstPointer",
    "Output",
};

Config::OverrideSpec::OverrideSpec( const std::string &value, const std::string &option )
{
    if( value.empty() )
    {
        std::string errorMsg = "Override option spec cannot be empty <" + option + ">";
        throw std::runtime_error( errorMsg );
    }

    size_t sepPos = value.find('/');
    if( sepPos != std::string::npos )
    {
        m_type = value.substr(0, sepPos);
        m_argExprMod = value.substr(sepPos+1);
        if( m_type.empty() )
        {
            std::string errorMsg = "Override option type cannot be empty <" + option + ">";
            throw std::runtime_error( errorMsg );
        }
        if( m_argExprMod.empty() )
        {
            std::string errorMsg = "Override option argument expression cannot be empty if specified <" + option + ">";
            throw std::runtime_error( errorMsg );
        }
        if( m_argExprMod.find('$') == std::string::npos )
        {
            std::string errorMsg = "Override option argument expression does not contain parameter name placeholder ($) <" + option + ">";
            throw std::runtime_error( errorMsg );
        }
    }
    else
    {
        m_type = value;
    }

    if( validOverrideTypes.count( m_type ) == 0 )
    {
        std::string errorMsg = "Invalid override option type <" + option + ">.";
        throw std::runtime_error( errorMsg );
    }
}

Config::OverrideMap::OverrideMap( const std::vector<std::string> &options )
{
    for( const std::string &option : options )
    {
        size_t sepPos = option.find('=');
        if( sepPos != std::string::npos )
        {
            std::string key = option.substr(0, sepPos);
            if( key.empty() )
            {
                std::string errorMsg = "Override option key cannot be empty <" + option + ">.";
                throw std::runtime_error( errorMsg );
            }

            Config::OverrideSpec spec = Config::OverrideSpec( option.substr(sepPos+1), option );

            if( !m_map.emplace( key, spec ).second )
            {
                std::string errorMsg = "Override option key <" + key + "> can only be passed once.";
                throw std::runtime_error( errorMsg );
            }
        }
        else
        {
            std::string errorMsg = "Invalid override option <" + option + ">.";
            throw std::runtime_error( errorMsg );
        }
    }
}

const Config::OverrideSpec* Config::OverrideMap::GetOverride( const std::string& key ) const
{
    OverrideMapType::const_iterator it = m_map.find( key );
    return ( it != m_map.end() ) ? &(it->second) : NULL;
}

const std::string& Config::OverrideSpec::GetType() const
{
    return m_type;
}

void Config::OverrideSpec::UpdateArgExpr( std::string& argExpr ) const
{
    if( !m_argExprMod.empty() )
    {
        std::string ret = m_argExprMod;
        size_t placeholderPos = m_argExprMod.find('$');

        argExpr = ret.replace(placeholderPos, 1, argExpr);
    }
}
