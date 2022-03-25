/**
 * @file
 * @brief      Header of Parser class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_PARSER_HPP_
#define CPPUMOCKGEN_PARSER_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <filesystem>

#include "Function.hpp"

class Config;

/**
 * Class used to parse C/C++ headers and generate mocks and expectations.
 */
class Parser
{
public:
    /**
     * Default constructor
     */
    Parser() : m_interpretAsCpp(false) {}

    /**
     * Parses the C/C++ header located in @p inputFilename.
     *
     * @param[in] inputFilepath Filename for the C/C++ header file
     * @param[in] config Configuration to be used during mock generation
     * @param[in] isCppHeader Indicates if the input file is recognized as a C++ header
     * @param[in] includePaths List of paths to search for included header files
     * @param[out] error Stream where errors will be written
     * @return @c true if the input file could be parsed successfully, @c false otherwise
     */
    bool Parse( const std::filesystem::path &inputFilepath, const Config &config, bool isCppHeader,
                const std::vector<std::string> &includePaths, std::ostream &error );

    /**
     * Generates mocked functions for the C/C++ header parsed previously.
     *
     * @param[in] genOpts String containing the generation options
     * @param[in] userCode String containing the user code existing in previously existing output file
     * @param[in] baseDirPath Base directory to reference input file in include directives
     * @param[out] output Stream where the generated mocks will be written
     */
    void GenerateMock( const std::string &genOpts, const std::string &userCode, const std::filesystem::path &baseDirPath,
                       std::ostream &output ) const noexcept;

    /**
     * Generates expectation functions header for the C/C++ header parsed previously.
     *
     * @param[in] genOpts String containing the generation options
     * @param[in] baseDirPath Base directory to reference input file in include directives
     * @param[out] output Stream where the generated expectations header will be written
     */
    void GenerateExpectationHeader( const std::string &genOpts, const std::filesystem::path &baseDirPath, std::ostream &output ) const noexcept;

    /**
     * Generates expectation functions implementation for the C/C++ header parsed previously.
     *
     * @param[in] genOpts String containing the generation options
     * @param[in] headerFilepath Filename for the expectation functions header file
     * @param[out] output Stream where the generated expectations implementation will be written
     */
    void GenerateExpectationImpl( const std::string &genOpts, const std::filesystem::path &headerFilepath, std::ostream &output ) const noexcept;

private:
    void GenerateFileHeading( const std::string &genOpts, std::ostream &output, bool hasUserCode ) const noexcept;

    std::vector<std::unique_ptr<const Function>> m_functions;
    std::filesystem::path m_inputFilePath;
    bool m_interpretAsCpp;
};

#endif // header guard
