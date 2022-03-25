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

#define USER_CODE_BEGIN "CPPUMOCKGEN_USER_CODE_BEGIN"
#define USER_CODE_END   "CPPUMOCKGEN_USER_CODE_END"

class OutputFileParser
{
public:
    OutputFileParser();

    void Parse( const std::filesystem::path &filepath );

    const std::string& GetUserCode() const;

private:
    void ParseLine( const std::string &line );

    bool m_captureUserCode;
    std::string m_userCode;
};

#endif // header guard
