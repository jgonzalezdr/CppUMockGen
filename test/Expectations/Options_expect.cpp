/*
 * This file has been auto-generated by CppUMockGen v0.6.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -t "#const std::string &=String~$.c_str()" -t "@std::string=String"
 */

#include "Options_expect.hpp"

namespace expect { namespace Options$ {
MockExpectedCall& Options$ctor()
{
    return Options$ctor(1);
}
MockExpectedCall& Options$ctor(unsigned int __numCalls__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::Options");
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& Parse(CppUMockGen::Parameter<const Options*> __object__, CppUMockGen::Parameter<int> argc, CppUMockGen::Parameter<const char **> argv)
{
    return Parse(1, __object__, argc, argv);
}
MockExpectedCall& Parse(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, CppUMockGen::Parameter<int> argc, CppUMockGen::Parameter<const char **> argv)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::Parse");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    if(argc.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withIntParameter("argc", argc.getValue()); }
    if(argv.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withPointerParameter("argv", argv.getValue()); }
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& Parse(CppUMockGen::Parameter<const Options*> __object__, CppUMockGen::Parameter<const char*> options)
{
    return Parse(1, __object__, options);
}
MockExpectedCall& Parse(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, CppUMockGen::Parameter<const char*> options)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::Parse");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    if(options.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withStringParameter("options", options.getValue()); }
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& IsHelpRequested(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return IsHelpRequested(1, __object__, __return__);
}
MockExpectedCall& IsHelpRequested(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::IsHelpRequested");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetHelpText(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetHelpText(1, __object__, __return__);
}
MockExpectedCall& GetHelpText(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetHelpText");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& IsVersionRequested(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return IsVersionRequested(1, __object__, __return__);
}
MockExpectedCall& IsVersionRequested(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::IsVersionRequested");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetInputPath(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetInputPath(1, __object__, __return__);
}
MockExpectedCall& GetInputPath(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetInputPath");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetBaseDirectory(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetBaseDirectory(1, __object__, __return__);
}
MockExpectedCall& GetBaseDirectory(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetBaseDirectory");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& IsMockRequested(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return IsMockRequested(1, __object__, __return__);
}
MockExpectedCall& IsMockRequested(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::IsMockRequested");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetMockOutputPath(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetMockOutputPath(1, __object__, __return__);
}
MockExpectedCall& GetMockOutputPath(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetMockOutputPath");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& IsExpectationsRequested(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return IsExpectationsRequested(1, __object__, __return__);
}
MockExpectedCall& IsExpectationsRequested(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::IsExpectationsRequested");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetExpectationsOutputPath(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetExpectationsOutputPath(1, __object__, __return__);
}
MockExpectedCall& GetExpectationsOutputPath(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetExpectationsOutputPath");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& IsRegenerationRequested(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return IsRegenerationRequested(1, __object__, __return__);
}
MockExpectedCall& IsRegenerationRequested(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::IsRegenerationRequested");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& InterpretAsCpp(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return InterpretAsCpp(1, __object__, __return__);
}
MockExpectedCall& InterpretAsCpp(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::InterpretAsCpp");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetLanguageStandard(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetLanguageStandard(1, __object__, __return__);
}
MockExpectedCall& GetLanguageStandard(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetLanguageStandard");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& UseUnderlyingTypedef(CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    return UseUnderlyingTypedef(1, __object__, __return__);
}
MockExpectedCall& UseUnderlyingTypedef(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, bool __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::UseUnderlyingTypedef");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetTypeOverrides(CppUMockGen::Parameter<const Options*> __object__, std::vector<std::string> &__return__)
{
    return GetTypeOverrides(1, __object__, __return__);
}
MockExpectedCall& GetTypeOverrides(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, std::vector<std::string> &__return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetTypeOverrides");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetIncludePaths(CppUMockGen::Parameter<const Options*> __object__, std::vector<std::string> &__return__)
{
    return GetIncludePaths(1, __object__, __return__);
}
MockExpectedCall& GetIncludePaths(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, std::vector<std::string> &__return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetIncludePaths");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(static_cast<const void*>(&__return__));
    return __expectedCall__;
}
} }

namespace expect { namespace Options$ {
MockExpectedCall& GetGenerationOptions(CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    return GetGenerationOptions(1, __object__, __return__);
}
MockExpectedCall& GetGenerationOptions(unsigned int __numCalls__, CppUMockGen::Parameter<const Options*> __object__, const char* __return__)
{
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Options::GetGenerationOptions");
    if(!__object__.isIgnored()) { __expectedCall__.onObject(const_cast<Options*>(__object__.getValue())); }
    __expectedCall__.andReturnValue(__return__);
    return __expectedCall__;
}
} }

