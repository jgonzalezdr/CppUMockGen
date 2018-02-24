
CppUMockGen
===========
Mock generator for [CppUTest/CppUMock](http://cpputest.github.io/).

[![Build status](https://ci.appveyor.com/api/projects/status/3ch50mcu29pgv5st/branch/master?svg=true)](https://ci.appveyor.com/project/jgonzalezdr/cppumockgen/branch/master)
[![codecov](https://codecov.io/gh/jgonzalezdr/CppUMockGen/branch/master/graph/badge.svg)](https://codecov.io/gh/jgonzalezdr/CppUMockGen)

## What is CppUMockGen?

CppUMockGen generates automatically mocks for member and non-member functions defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

## Download Binaries

* [GitHub Releases](https://github.com/jgonzalezdr/CppUMockGen/releases)
* [Last successful Windows build of the master branch](https://ci.appveyor.com/api/projects/jgonzalezdr/CppUMockGen/artifacts/CppUMockGen.exe?branch=master&job=Environment:%20Platform=MinGW64,%20PlatformToolset=6.3.0,%20Configuration=Release,%20Test=False,%20PublishArtifacts=True,%20APPVEYOR_BUILD_WORKER_IMAGE=Visual%20Studio%202015)

## Getting Started

###### Basic Command-Line Options

`CppUMockGen [OPTION...] [<input>]`

| OPTION                            | Description                                   |
| -                                 | -                                             |
| `-i, --input <input> `            | Input file                                    |
| `-m, --mock-output <mock-output>` | Mock output path                              |
| `-x, --cpp`                       | Force interpretation of the input file as C++ |
| `-I, --include-path <path>`       | Include path                                  |
| `-p, --param-override <expr>`     | Override parameter type                       |
| `-t, --type-override <expr>`      | Override generic type                         |
| `-h, --help`                      | Print help                                    |

To generate a mock from a header file containing the functions that you want to mock, just pass the path to the header file as input (first parameter or explicitly with the `-i` / `--input` option) and the path where you want the file with the mocked functions to be generated as output (second parameter or explicitly with the `-m` / `--mock-output` option). If the option parameter is empty or is a directory path (i.e. ending with a path separator) then the output file name will be deduced from the input file name by replacing its extension by *"_mock.cpp"*. If the option parameter is **'@'**, the mock is printed to the console.

CppUMock by default interprets header files with the extensions .hh, .hpp or .hxx as C\++. Other extensions are interpreted by default as C. To force the interpretation of a header file as C++ use the `-x` / `--cpp` option.

CppUMock, just as any C/C++ compiler, needs to know where to find other include files referenced by the input file in order to interpret it properly. Pass the paths to the necessary include directories by using the `-I` / `--include-path` option. Like with most compilers, you may use this option several times to indicate multiple include directories.

CppUMockGen deduces the data types to use with CppUMock from the actual function parameters and return types. If the API that you are mocking is well designed (e.g. pointers to non-const values are not used for input parameters), CppUMockGen will guess properly in most cases the correct types. Nevertheless, mocked data types can be overriden by using `-p` / `--param-override` options to override the type to use for specific function's parameters and return types, and using `-t` / `--type-override` options to override the type to use for matching parameter or return types in any mocked function.

For more information check the [CppUMockGen Usage Manual](MANUAL.md).

## Building from Source

### Requirements

- [cmake](https://cmake.org/) (tested with v3.7.1)
- [clang](http://releases.llvm.org/download.html) (tested with v5.0.1)
- A C/C++ compiler, either:
  - [MinGW-w64](https://sourceforge.net/projects/mingw-w64/) (tested with x86_64-6.4.0-posix-seh-rt_v5-rev0)
  - [Microsoft Visual Studio](https://www.visualstudio.com/es/downloads/) (tested with Visual Studio Community 2015 and 2017)
- [CppUTest](http://cpputest.github.io/) [Optional, not needed if tests are disabled] (tested with v3.7.1 and v3.8)
- [LCOV](https://github.com/jgonzalezdr/lcov/releases) [Optional, needed if tests and coverage are enabled using GCC/MinGW] (tested with v1.11.1w)
- [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) [Optional, needed if tests and coverage are enabled using Visual Studio 2017] (tested with v0.9.6.1)