/*
 * This file has been auto-generated by CppUMockGen v0.5.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -t "#const CXCursor &=ConstPointer~&$" -t "#const Config &=ConstPointer~&$" -t "@std::string=String" -t "#const std::string &=String~$.c_str()"
 */

#include <CppUMockGen.hpp>

#include "Function.hpp"

#include <CppUTestExt/MockSupport.h>

namespace expect { namespace Function$ {
MockExpectedCall& Function$ctor();
MockExpectedCall& Function$ctor(unsigned int __numCalls__);
} }

namespace expect { namespace Function$ {
MockExpectedCall& Function$dtor(CppUMockGen::Parameter<const Function*> __object__ = ::CppUMockGen::IgnoreParameter::YES);
MockExpectedCall& Function$dtor(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__ = ::CppUMockGen::IgnoreParameter::YES);
} }

namespace expect { namespace Function$ {
MockExpectedCall& Parse(CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const void*> cursor, CppUMockGen::Parameter<const void*> config, bool __return__);
MockExpectedCall& Parse(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const void*> cursor, CppUMockGen::Parameter<const void*> config, bool __return__);
} }

namespace expect { namespace Function$ {
MockExpectedCall& GenerateMock(CppUMockGen::Parameter<const Function*> __object__, const char* __return__);
MockExpectedCall& GenerateMock(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__, const char* __return__);
} }

namespace expect { namespace Function$ {
MockExpectedCall& GenerateExpectation(CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<bool> proto, const char* __return__);
MockExpectedCall& GenerateExpectation(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<bool> proto, const char* __return__);
} }

namespace expect { namespace Function$ {
MockExpectedCall& IsMockable(CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const void*> cursor, bool __return__);
MockExpectedCall& IsMockable(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const void*> cursor, bool __return__);
} }

namespace expect { namespace Function$ {
MockExpectedCall& GetExpectationFunctionName(CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const char*> functionName, const char* __return__);
MockExpectedCall& GetExpectationFunctionName(unsigned int __numCalls__, CppUMockGen::Parameter<const Function*> __object__, CppUMockGen::Parameter<const char*> functionName, const char* __return__);
} }

