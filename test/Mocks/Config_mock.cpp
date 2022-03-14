/*
 * This file has been auto-generated by CppUMockGen v0.5.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -s c++17 -t "#const std::string &=String~$.c_str()"
 */

#include "Config.hpp"

#include <CppUTestExt/MockSupport.h>

Config::OverrideMap::OverrideMap( const std::vector<std::string> &options )
{
}

Config::OverrideSpec::OverrideSpec(const std::string & value, bool isReturn)
{
    mock().actualCall("Config::OverrideSpec::OverrideSpec").withStringParameter("value", value.c_str()).withBoolParameter("isReturn", isReturn);
}

MockedType Config::OverrideSpec::GetType() const noexcept
{
    return static_cast<MockedType>(mock().actualCall("Config::OverrideSpec::GetType").onObject(this).returnIntValue());
}

const std::string & Config::OverrideSpec::GetExposedTypeName() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetExposedTypeName").onObject(this).returnConstPointerValue());
}

const std::string & Config::OverrideSpec::GetExprModFront() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetExprModFront").onObject(this).returnConstPointerValue());
}

const std::string & Config::OverrideSpec::GetExprModBack() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetExprModBack").onObject(this).returnConstPointerValue());
}

const bool Config::OverrideSpec::HasSizeExprPlaceholder() const noexcept
{
    return mock().actualCall("Config::OverrideSpec::HasSizeExprPlaceholder").onObject(this).returnBoolValue();
}

const std::string & Config::OverrideSpec::GetSizeExprFront() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetSizeExprFront").onObject(this).returnConstPointerValue());
}

const std::string & Config::OverrideSpec::GetSizeExprBack() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetSizeExprBack").onObject(this).returnConstPointerValue());
}

const std::string & Config::OverrideSpec::GetExpectationArgTypeName() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::OverrideSpec::GetExpectationArgTypeName").onObject(this).returnConstPointerValue());
}

Config::Config(bool interpretAsCpp, const std::string & languageStandard, bool useUnderlyingTypedefType, const std::vector<std::string> & typeOverrideOptions)
: m_typeOverrideMap( std::vector<std::string>() )
{
    mock().actualCall("Config::Config").withBoolParameter("interpretAsCpp", interpretAsCpp).withStringParameter("languageStandard", languageStandard.c_str()).withBoolParameter("useUnderlyingTypedefType", useUnderlyingTypedefType).withParameterOfType("std::vector<std::string>", "typeOverrideOptions", &typeOverrideOptions);
}

bool Config::InterpretAsCpp() const noexcept
{
    return mock().actualCall("Config::InterpretAsCpp").onObject(this).returnBoolValue();
}

const std::string & Config::GetLanguageStandard() const noexcept
{
    return *static_cast<const std::string*>(mock().actualCall("Config::GetLanguageStandard").onObject(this).returnConstPointerValue());
}

bool Config::UseUnderlyingTypedefType() const noexcept
{
    return mock().actualCall("Config::UseUnderlyingTypedefType").onObject(this).returnBoolValue();
}

const Config::OverrideSpec * Config::GetTypeOverride(const std::string & key) const noexcept
{
    return static_cast<const Config::OverrideSpec*>(mock().actualCall("Config::GetTypeOverride").onObject(this).withStringParameter("key", key.c_str()).returnConstPointerValue());
}

