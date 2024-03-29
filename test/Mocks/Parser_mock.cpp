/*
 * This file has been auto-generated by CppUMockGen v0.6.
 *
 * Only contents in the user code section will be preserved if it is regenerated!!!
 *
 * Generation options: -s c++17 -t "Parser::Parse#error=Pointer~&$" -t "#const std::string &=String~$.c_str()" -t "#const Config &=ConstPointer~&$" -t "#std::ostream &=OutputOfType:std::ostream<std::string~&$" -t "#const std::filesystem::path &=String~$.generic_string().c_str()"
 */

#include "Parser.hpp"

#include <CppUTestExt/MockSupport.h>

// CPPUMOCKGEN_USER_CODE_BEGIN
// CPPUMOCKGEN_USER_CODE_END

bool Parser::Parse(const std::filesystem::path & inputFilepath, const Config & config, bool isCppHeader, const std::vector<std::string> & includePaths, std::ostream & error)
{
    return mock().actualCall("Parser::Parse").onObject(this).withStringParameter("inputFilepath", inputFilepath.generic_string().c_str()).withConstPointerParameter("config", &config).withBoolParameter("isCppHeader", isCppHeader).withParameterOfType("std::vector<std::string>", "includePaths", &includePaths).withPointerParameter("error", &error).returnBoolValue();
}

void Parser::GenerateMock(const std::string & genOpts, const std::string & userCode, const std::filesystem::path & baseDirPath, std::ostream & output) const noexcept
{
    mock().actualCall("Parser::GenerateMock").onObject(this).withStringParameter("genOpts", genOpts.c_str()).withStringParameter("userCode", userCode.c_str()).withStringParameter("baseDirPath", baseDirPath.generic_string().c_str()).withOutputParameterOfType("std::ostream", "output", &output);
}

void Parser::GenerateExpectationHeader(const std::string & genOpts, const std::filesystem::path & baseDirPath, std::ostream & output) const noexcept
{
    mock().actualCall("Parser::GenerateExpectationHeader").onObject(this).withStringParameter("genOpts", genOpts.c_str()).withStringParameter("baseDirPath", baseDirPath.generic_string().c_str()).withOutputParameterOfType("std::ostream", "output", &output);
}

void Parser::GenerateExpectationImpl(const std::string & genOpts, const std::filesystem::path & headerFilepath, std::ostream & output) const noexcept
{
    mock().actualCall("Parser::GenerateExpectationImpl").onObject(this).withStringParameter("genOpts", genOpts.c_str()).withStringParameter("headerFilepath", headerFilepath.generic_string().c_str()).withOutputParameterOfType("std::ostream", "output", &output);
}

