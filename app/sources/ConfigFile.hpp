/**
 * @file
 * @brief      Header of ConfigFile class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_CONFIGFILE_HPP_
#define CPPUMOCKGEN_CONFIGFILE_HPP_

namespace cxxopts
{
class Options;
}

void AddConfigFileOptions( cxxopts::Options &options );

void ProcessConfigFiles( cxxopts::Options &options );

#endif // header guard
