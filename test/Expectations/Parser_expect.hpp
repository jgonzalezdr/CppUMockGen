/*
 * This file has been auto-generated by CppUTestMock v0.4.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -s c++17 -t Parser::Parse#error=Pointer~&$ -t "#const std::string &=String~$.c_str()" -t "#const Config &=ConstPointer~&$" -t "#std::ostream &=OutputOfType:std::ostream<std::string~&$" -t "#const std::filesystem::path &=String~$.generic_string().c_str()" 
 */

#include <CppUMockGen.hpp>

#include "Parser.hpp"

#include <CppUTestExt/MockSupport.h>

namespace expect { namespace Parser$ {
MockExpectedCall& Parse(CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> inputFilepath, CppUMockGen::Parameter<const void*> config, CppUMockGen::Parameter<bool> interpretAsCpp, CppUMockGen::Parameter<const char*> languageStandard, CppUMockGen::Parameter<const std::vector<std::string> &> includePaths, CppUMockGen::Parameter<void*> error, bool __return__);
MockExpectedCall& Parse(unsigned int __numCalls__, CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> inputFilepath, CppUMockGen::Parameter<const void*> config, CppUMockGen::Parameter<bool> interpretAsCpp, CppUMockGen::Parameter<const char*> languageStandard, CppUMockGen::Parameter<const std::vector<std::string> &> includePaths, CppUMockGen::Parameter<void*> error, bool __return__);
} }

namespace expect { namespace Parser$ {
MockExpectedCall& GenerateMock(CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, const std::string* output);
MockExpectedCall& GenerateMock(unsigned int __numCalls__, CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, const std::string* output);
} }

namespace expect { namespace Parser$ {
MockExpectedCall& GenerateExpectationHeader(CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, const std::string* output);
MockExpectedCall& GenerateExpectationHeader(unsigned int __numCalls__, CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, const std::string* output);
} }

namespace expect { namespace Parser$ {
MockExpectedCall& GenerateExpectationImpl(CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, CppUMockGen::Parameter<const char*> headerFilepath, const std::string* output);
MockExpectedCall& GenerateExpectationImpl(unsigned int __numCalls__, CppUMockGen::Parameter<const Parser*> __object__, CppUMockGen::Parameter<const char*> genOpts, CppUMockGen::Parameter<const char*> headerFilepath, const std::string* output);
} }

