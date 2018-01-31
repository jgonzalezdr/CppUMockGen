#ifndef MOCKGENERATOR_HPP_
#define MOCKGENERATOR_HPP_

#include <vector>
#include <string>
#include <sstream>

#include "Config.hpp"

bool GenerateMock( const std::string &inputFilename, const Config &config, bool interpretAsCpp,
                   const std::vector<std::string> &includePaths, std::ostream &output, std::ostream &error );

#endif // header guard
