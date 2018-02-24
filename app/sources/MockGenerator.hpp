#ifndef CPPUMOCKGEN_MOCKGENERATOR_HPP_
#define CPPUMOCKGEN_MOCKGENERATOR_HPP_

#include <vector>
#include <string>
#include <sstream>

#include "Config.hpp"

/**
 * Generates mocked functions for the C/C++ header located in @p inputFilename and writes them to @p output.
 *
 * @param inputFilename [in] Filename for the C/C++ header file
 * @param config [in] Configuration to be used during mock generation
 * @param interpretAsCpp [in] Forces interpreting the header file as C++
 * @param includePaths [in] List of paths to search for included header files
 * @param genOpts [in] String containing the generation options
 * @param output [out] Stream where the generated mocks will be written
 * @param error [out] Stream where errors will be written
 * @return @c true if the mocks could be generated successfully, @c false otherwise
 */
bool GenerateMock( const std::string &inputFilename, const Config &config, bool interpretAsCpp,
                   const std::vector<std::string> &includePaths, const std::string &genOpts,
                   std::ostream &output, std::ostream &error );

#endif // header guard
