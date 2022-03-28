/**
 * @file
 * @brief      Declaration of Options class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2022 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CppUMockGen_Options_hpp_
#define CppUMockGen_Options_hpp_

#include <cxxopts.hpp>

#include <vector>
#include <string>

class Options
{
public:
    /**
     * Constructs a Options object.
     */
    Options();

    /**
     * Parse the options from the command line
     * 
     * @param[in] argc Number of arguments
     * @param[in] argv Array of arguments
     */
    void Parse( int argc, const char* argv[] );

    /**
     * Parse the options from a string.
     * 
     * @param[in] options String with options
     */
    void Parse( const std::string &options );

    bool IsHelpRequested() const;

    std::string GetHelpText() const;

    bool IsVersionRequested() const;

    std::string GetInputPath() const;

    std::string GetBaseDirectory() const;

    bool IsMockRequested() const;

    std::string GetMockOutputPath() const;

    bool IsExpectationsRequested() const;

    std::string GetExpectationsOutputPath() const;

    bool IsRegenerationRequested() const;

    bool InterpretAsCpp() const;

    std::string GetLanguageStandard() const;

    bool UseUnderlyingTypedef() const;

    std::vector<std::string> GetTypeOverrides() const;

    std::vector<std::string> GetIncludePaths() const;

    std::string GetGenerationOptions() const;

private:
    cxxopts::Options m_options;
};

#endif // header guard
