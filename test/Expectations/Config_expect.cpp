/*
 * This file has been auto-generated by CppUMockGen v0.5.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -s c++17 -t "#const std::string &=String~$.c_str()"
 */

#include "Config_expect.hpp"

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& OverrideSpec$ctor(CppUMockGen::Parameter<const char*> value, CppUMockGen::Parameter<bool> isReturn)
{
    return OverrideSpec$ctor(1, value, isReturn);
}
MockExpectedCall& OverrideSpec$ctor(unsigned int __numCalls__, CppUMockGen::Parameter<const char*> value, CppUMockGen::Parameter<bool> isReturn)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::OverrideSpec");
    if(value.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withStringParameter("value", value.getValue()); }
    if(isReturn.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withBoolParameter("isReturn", isReturn.getValue()); }
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetType(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, MockedType __return__)
{
    return GetType(1, __object__, __return__);
}
MockExpectedCall& GetType(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, MockedType __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetType");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<int>(__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetExposedTypeName(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetExposedTypeName(1, __object__, __return__);
}
MockExpectedCall& GetExposedTypeName(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetExposedTypeName");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetExprModFront(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetExprModFront(1, __object__, __return__);
}
MockExpectedCall& GetExprModFront(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetExprModFront");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetExprModBack(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetExprModBack(1, __object__, __return__);
}
MockExpectedCall& GetExprModBack(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetExprModBack");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& HasSizeExprPlaceholder(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const bool __return__)
{
    return HasSizeExprPlaceholder(1, __object__, __return__);
}
MockExpectedCall& HasSizeExprPlaceholder(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::HasSizeExprPlaceholder");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetSizeExprFront(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetSizeExprFront(1, __object__, __return__);
}
MockExpectedCall& GetSizeExprFront(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetSizeExprFront");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetSizeExprBack(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetSizeExprBack(1, __object__, __return__);
}
MockExpectedCall& GetSizeExprBack(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetSizeExprBack");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ { namespace OverrideSpec$ {
MockExpectedCall& GetExpectationArgTypeName(CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    return GetExpectationArgTypeName(1, __object__, __return__);
}
MockExpectedCall& GetExpectationArgTypeName(unsigned int __numCalls__, CppUMockGen::Parameter<const Config::OverrideSpec*> __object__, const std::string & __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::OverrideSpec::GetExpectationArgTypeName");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config::OverrideSpec*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} } }

namespace expect { namespace Config$ {
MockExpectedCall& Config$ctor(CppUMockGen::Parameter<bool> useUnderlyingTypedefType, CppUMockGen::Parameter<const std::vector<std::string> &> typeOverrideOptions)
{
    return Config$ctor(1, useUnderlyingTypedefType, typeOverrideOptions);
}
MockExpectedCall& Config$ctor(unsigned int __numCalls__, CppUMockGen::Parameter<bool> useUnderlyingTypedefType, CppUMockGen::Parameter<const std::vector<std::string> &> typeOverrideOptions)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::Config");
    if(useUnderlyingTypedefType.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withBoolParameter("useUnderlyingTypedefType", useUnderlyingTypedefType.getValue()); }
    if(typeOverrideOptions.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withParameterOfType("std::vector<std::string>", "typeOverrideOptions", &typeOverrideOptions.getValue()); }
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} }

namespace expect { namespace Config$ {
MockExpectedCall& UseUnderlyingTypedefType(CppUMockGen::Parameter<const Config*> __object__, bool __return__)
{
    return UseUnderlyingTypedefType(1, __object__, __return__);
}
MockExpectedCall& UseUnderlyingTypedefType(unsigned int __numCalls__, CppUMockGen::Parameter<const Config*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::UseUnderlyingTypedefType");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Config$ {
MockExpectedCall& GetTypeOverride(CppUMockGen::Parameter<const Config*> __object__, CppUMockGen::Parameter<const char*> key, const Config::OverrideSpec * __return__)
{
    return GetTypeOverride(1, __object__, key, __return__);
}
MockExpectedCall& GetTypeOverride(unsigned int __numCalls__, CppUMockGen::Parameter<const Config*> __object__, CppUMockGen::Parameter<const char*> key, const Config::OverrideSpec * __return__)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Config::GetTypeOverride");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Config*>(__object__.getValue())); }
    if(key.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withStringParameter("key", key.getValue()); }
    __expectedCall__.andReturnValue(static_cast<const void*>(__return__));
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} }

