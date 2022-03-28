/**
 * @file
 * @brief      Declaration of OutputFileParser class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2022 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CppUMockGen_OutputFileParser_hpp_
#define CppUMockGen_OutputFileParser_hpp_

#include <filesystem>
#include <string>

#define USER_CODE_BEGIN             "CPPUMOCKGEN_USER_CODE_BEGIN"
#define USER_CODE_END               "CPPUMOCKGEN_USER_CODE_END"
#define GENERATION_OPTIONS_LABEL    "Generation options:"

class OutputFileParser
{
public:
    OutputFileParser();

    void Parse( const std::filesystem::path &filepath );

    const std::string& GetUserCode() const;

    const std::string& GetGenerationOptions() const;

private:
    void ParseLine( const std::string &line, bool &captureUserCode );

    std::string m_userCode;
    std::string m_generationOptions;
};

#endif // header guard
