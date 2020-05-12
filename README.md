
CppUMockGen
===========
Mock generator for [CppUTest/CppUMock](http://cpputest.github.io/).

[![Build status](https://ci.appveyor.com/api/projects/status/3ch50mcu29pgv5st/branch/master?svg=true)](https://ci.appveyor.com/project/jgonzalezdr/cppumockgen/branch/master)
[![codecov](https://codecov.io/gh/jgonzalezdr/CppUMockGen/branch/master/graph/badge.svg)](https://codecov.io/gh/jgonzalezdr/CppUMockGen)

## What is CppUMockGen?

CppUMockGen generates automatically mocks for member and non-member functions defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

CppUMockGen additionally generates automatically expectation functions to improve and ease the declaration of expectations for functions in tests, making expectation declarations shorter and less error prone.

## Why using CppUMockGen?

In real world projects with many components (e.g. classes) which interfaces often change, there will be many mocked functions and unit tests that have to be created and maintained.

CppUTest/CppUMock provides a very powerful mechanism to define mocks for C/C\++ functions using `MockSupport::actualCall` and to declare function call expectations in unit tests using `MockSupport::expectOneCall` / `MockSupport::expectNCalls`.

However, defining and maintaining these mock definitions is a cumbersome, tedious and error prone task: the user has to ensure that the C/C\++ signature of the mocking function is in sync with the function declaration (at least the compiler will moan if not), keep the symbolic ("stringified") name of the function in sync with the actual C/C\++ name, declare each parameter individually and keep the parameter's symbolic names and data type usage in sync with the function parameters declarations, etc. Additionally, the expected call declarations have also to be maintained, keeping the symbolic names of the functions and parameters in sync with the ones used in mock definitions, etc.

The previously described manual tasks are highly systematic, and can be automated using CppUMockGen: this tool can generate automatically mock definitions for functions declared in C/C\++ header files, and also their corresponding expectation functions that encapsulate the expectation cumbersome details.

#### Example

Assume that we have the following function that we want to test:

> _FunctionToTest.h_
> ```cpp
> bool FunctionToTest( char *p );
> ```

And that this function calls another function, that we need to mock:

> _FunctionToMock.h_
> ```cpp
> const char* FunctionToMock( const char *p1, int p2 );
> ```

With CppUMockGen we can generate the following mock:

> _FunctionToMock_mock.cpp_
> ```cpp
> const char * FunctionToMock(const char * p1, int p2)
> {
>     return mock().actualCall("FunctionToMock")
>         .withStringParameter("p1", p1)
>         .withIntParameter("p2", p2)
>         .returnStringValue();
> }
> ```

Additionally, with CppUMockGen we can generate the following expectation:

> _FunctionToMock_expect.h_
> ```cpp
> namespace expect {
> MockExpectedCall& FunctionToMock(CppUMockGen::Parameter<const char *> p1,
>                                  CppUMockGen::Parameter<int> p2,
>                                  const char * __return__);
> MockExpectedCall& FunctionToMock(unsigned int __numCalls__,
>                                  CppUMockGen::Parameter<const char *> p1,
>                                  CppUMockGen::Parameter<int> p2,
>                                  const char * __return__);
> }
> ```

Eventually, we can use these generated mocks and expectations in an unit test like this, which checks that the tested function calls the mocked function once, with the string "ABC" passed as the first parameter, ignoring the value of the second parameter, then gets the string "123" as the result of the call, and finally returns `true`:

> _FunctionToTest_test.cpp_
> ```cpp
> using CppUMockGen::IgnoreParameter;
> 
> TEST( TestSuite, FunctionToTest )
> {
>     // Prepare
>     expect::FunctionToMock( "ABC",                 // Parameter p1
>                             IgnoreParameter::YES,  // Parameter p2
>                             "123" );               // Return value
>
>     // Exercise
>     bool ret = FunctionToTest( "ABC" );
>
>     // Verify
>     CHECK_EQUAL( true, ret );
>     mock().checkExpectations();
>
>     // Cleanup
> }
> ```

## Getting Started

###### Basic Command-Line Options

`CppUMockGen [OPTION...] [<input>]`

| OPTION                                | Description                                   |
| -                                     | -                                             |
| `-i, --input <input> `                | Input file                                    |
| `-m, --mock-output <mock-output>`     | Mock output path                              |
| `-e, --expect-output <expect-output>` | Expectation output path                       |
| `-x, --cpp`                           | Force interpretation of the input file as C++ |
| `-s, --std`                           | Set language standard (c\+\+14, c\+\+17, etc.)|
| `-I, --include-path <path>`           | Include path                                  |
| `-t, --type-override <expr>`          | Override generic type                         |
| `-f, --config-file <file>`            | Configuration file to be parsed for options   |
| `-v, --version`                       | Print version                                 |
| `-h, --help`                          | Print help                                    |

To generate a mock from a header file containing the functions that you want to mock, just pass the path to the header file as input in the first non-option argument or explicitly with the `-i` / `--input` option, and the path where you want the file with the mocked functions to be generated as output using the `-m` / `--mock-output` option.

To generate expectations, pass the path where you want the files with the expectation to be generated as output using the `-e` / `--expect-output` option (additionally to or instead of the `-m` option).

CppUMock, just as any C/C++ compiler, needs to know where to find other include files referenced by the input file in order to interpret it properly. Pass the paths to the necessary include directories by using the `-I` / `--include-path` option. Like with most compilers, you may use this option several times to indicate multiple include directories.

CppUMockGen deduces the data types to use with CppUMock from the actual function parameters and return types. If the API that you are mocking is well designed (e.g. pointers to non-const values are not used for input parameters), CppUMockGen will guess properly in most cases the correct types. Nevertheless, mocked data types can be overridden by using `-t` / `--type-override` options to indicate the type of function parameters or function returns. Type overriding can be defined in generic form, which apply to any function parameter or return types that match the specified type, or can be defined in function-specific form, which apply to parameters and return types of the specified function.

Since complex mocks will require a lot of override options, and many of them can be reused to generate mocks for other files, you can store options in a text file and load that file using the `-f` / `--config-file` option.

For more information check the [CppUMockGen Usage Manual](MANUAL.md).

## Download Binaries

* [GitHub Releases](https://github.com/jgonzalezdr/CppUMockGen/releases)
* [Last successful Windows build of the master branch](https://ci.appveyor.com/api/projects/jgonzalezdr/CppUMockGen/artifacts/Install%20CppUMockGen%20Experimental.exe?branch=master&job=Environment:%20Platform=MinGW64,%20PlatformToolset=6.3.0,%20Configuration=Release,%20Test=False,%20PublishArtifacts=True,%20GenerateInstaller=True,%20APPVEYOR_BUILD_WORKER_IMAGE=Visual%20Studio%202015)

## Building from Source

#### Requirements

- [cmake](https://cmake.org/) (tested with v3.17.2)
- [clang](http://releases.llvm.org/download.html) (tested with v10.0.0)
- A C/C++ compiler, either:
  - [MinGW-w64](https://sourceforge.net/projects/mingw-w64/) (tested with [TDM-GCC 9.2.0](https://jmeubank.github.io/tdm-gcc/download/), will not work with MinGW-w64 releases &lt;= 8.1.0)
  - [Microsoft Visual Studio](https://www.visualstudio.com/es/downloads/) (tested with Visual Studio Community 2019)
- [CppUTest](http://cpputest.github.io/) [Optional, not needed if tests are disabled] (requires at least [v3.9.alpha0](https://github.com/jgonzalezdr/cpputest/releases/download/v3.9.alpha0/cpputest-3.9.alpha0.zip))
- [LCOV for Windows](https://github.com/jgonzalezdr/lcov/releases) [Optional, needed if tests and coverage are enabled using GCC/MinGW] (requires at least [v1.15.alpha0w](https://github.com/jgonzalezdr/lcov/releases/download/v1.15.alpha0w/lcov-v1.15.alpha0w.zip))
- [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) [Optional, needed if tests and coverage are enabled using Visual Studio 2019] (tested with v0.9.8.0)

#### Build using TDD-GCC in Windows

To build CppUMockGen first clone this repository on your machine, and in the directory where you cloned it execute the following commands:

```powershell
mkdir build_mingw
cd build_mingw
cmake .. -G "MinGW Makefiles" -DCPPUTEST_HOME="<Path to your CppUTest home>"
mingw32-make
```

To execute the tests execute the following command in the _build_mingw_ directory:

```powershell
mingw32-make run_tests
```

#### Build using Visual Studio 2019

To build CppUMockGen first clone this repository on your machine, and in the directory where you cloned it execute the following commands:

```powershell
mkdir build_vs
cd build_vs
cmake .. -G "Visual Studio 16 2019" -DCPPUTEST_HOME="<Path to your CppUTest home>"
```

Eventually open the _CppUMockGen.Top.sln_ VS solution generated in the _build_vs_ directory, then compile the solution. To execute the tests, compile the _run_tests_ project.