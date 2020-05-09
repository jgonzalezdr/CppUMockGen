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
     * @param inputFilepath [in] Filename for the C/C++ header file
     * @param config [in] Configuration to be used during mock generation
     * @param interpretAsCpp [in] Forces interpreting the header file as C++
     * @param includePaths [in] List of paths to search for included header files
     * @param error [out] Stream where errors will be written
     * @return @c true if the input file could be parsed successfully, @c false otherwise
     */
    bool Parse( const std::filesystem::path &inputFilepath, const Config &config, bool interpretAsCpp,
                const std::string &languageStandard, const std::vector<std::string> &includePaths, std::ostream &error );

    /**
     * Generates mocked functions for the C/C++ header parsed previously.
     *
     * @param genOpts [in] String containing the generation options
     * @param output [out] Stream where the generated mocks will be written
     */
    void GenerateMock( const std::string &genOpts, std::ostream &output ) const noexcept;

    /**
     * Generates expectation functions header for the C/C++ header parsed previously.
     *
     * @param genOpts [in] String containing the generation options
     * @param output [out] Stream where the generated expectations header will be written
     */
    void GenerateExpectationHeader( const std::string &genOpts, std::ostream &output ) const noexcept;

    /**
     * Generates expectation functions implementation for the C/C++ header parsed previously.
     *
     * @param genOpts [in] String containing the generation options
     * @param headerFilepath [in] Filename for the expectation functions header file
     * @param output [out] Stream where the generated expectations implementation will be written
     */
    void GenerateExpectationImpl( const std::string &genOpts, const std::filesystem::path &headerFilepath, std::ostream &output ) const noexcept;

private:
    void GenerateFileHeading( const std::string &genOpts, std::ostream &output ) const noexcept;

    std::vector<std::unique_ptr<const Function>> m_functions;
    std::filesystem::path m_inputFilepath;
    bool m_interpretAsCpp;
};

#endif // header guard
