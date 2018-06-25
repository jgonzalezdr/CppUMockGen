
CppUMockGen
===========
Mock generator for [CppUTest/CppUMock](http://cpputest.github.io/).

[![Build status](https://ci.appveyor.com/api/projects/status/3ch50mcu29pgv5st/branch/master?svg=true)](https://ci.appveyor.com/project/jgonzalezdr/cppumockgen/branch/master)
[![codecov](https://codecov.io/gh/jgonzalezdr/CppUMockGen/branch/master/graph/badge.svg)](https://codecov.io/gh/jgonzalezdr/CppUMockGen)

## What is CppUMockGen?

CppUMockGen generates automatically mocks for member and non-member functions defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

CppUMockGen additionally generates automatically expectation helper functions to improve and ease the declaration of expectations for functions in tests, making expectation declarations shorter and less error prone.

## Why using CppUMockGen?

In real world projects with many components (e.g. classes) which interfaces often change, there will be many mocked functions and unit tests that have to be created and maintained.

CppUTest/CppUMock provides a very powerful mechanism to define mocks for C/C\++ functions using `MockSupport::actualCall` and to declare function call expectations in unit tests using `MockSupport::expectOneCall` / `MockSupport::expectNCalls`.

However, defining and maintaining these mock definitions is a cumbersome, tedious and error prone task: the user has to ensure that the C/C\++ signature of the mocking function is in sync with the function declaration (at least the compiler will moan if not), keep the symbolic ("stringified") name of the function in sync with the actual C/C\++ name, declare each parameter individually and keep the parameter's symbolic names and data type usage in sync with the function parameters declarations, etc. Additionally, the expected call declarations have also to be maintained, keeping the symbolic names of the functions and parameters in sync with the ones used in mock definitions, etc.

The previously described manual tasks are highly systematic, and can be automated using CppUMockGen: this tool can generate automatically mock definitions for functions declared in C/C\++ header files, and also their corresponding expectation helper functions that encapsulate the expectation cumbersome details.

#### Example

Assuming that we have the following function that we want to test:

###### MyFunction.h
```cpp
char* myFunction( const char *p );
```

CppUMockGen would generate the following mock:

###### MyFunction_mock.cpp
```cpp
char * myFunction(const char * p)
{
    return static_cast<char*>(mock().actualCall("myFunction").withConstPointerParameter("p", p).returnStringValue());
}
```

CppUMockGen would generate the following expectation helper function:

###### MyFunction_expect.h
```cpp
namespace expect {
MockExpectedCall& myFunction(CppUMockGen::Parameter<const char *> p, char * __return__);
}
```

###### MyFunction_expect.cpp
```cpp
namespace expect {
MockExpectedCall& myFunction(CppUMockGen::Parameter<const char *> p, char * __return__)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectOneCall("myFunction");
    if(p.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withConstPointerParameter("p", p); }
    __expectedCall__.andReturnValue(static_cast<void*>(__return__));
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __ignoreOtherParams__;
}
}
```

And we could use these in an unit test like this:

###### MyFunction_test.cpp
```cpp
TEST( myTestSuite, myOtherFunction )
{
    expect::myFunction("ABC", "123");
    CHECK_EQUAL( true, myOtherFunction("ABC") );
    mock().checkExpectations();
}
```

## Download Binaries

* [GitHub Releases](https://github.com/jgonzalezdr/CppUMockGen/releases)
* [Last successful Windows build of the master branch](https://ci.appveyor.com/api/projects/jgonzalezdr/CppUMockGen/artifacts/Install%20CppUMockGen%20Experimental.exe?branch=master&job=Environment:%20Platform=MinGW64,%20PlatformToolset=6.3.0,%20Configuration=Release,%20Test=False,%20PublishArtifacts=True,%20GenerateInstaller=True,%20APPVEYOR_BUILD_WORKER_IMAGE=Visual%20Studio%202015)

## Getting Started

###### Basic Command-Line Options

`CppUMockGen [OPTION...] [<input>]`

| OPTION                                | Description                                   |
| -                                     | -                                             |
| `-i, --input <input> `                | Input file                                    |
| `-m, --mock-output <mock-output>`     | Mock output path                              |
| `-e, --expect-output <expect-output>` | Expectation output path                              |
| `-x, --cpp`                           | Force interpretation of the input file as C++ |
| `-I, --include-path <path>`           | Include path                                  |
| `-p, --param-override <expr>`         | Override parameter type                       |
| `-t, --type-override <expr>`          | Override generic type                         |
| `-h, --help`                          | Print help                                    |

To generate a mock from a header file containing the functions that you want to mock, just pass the path to the header file as input in the first non-option parameter or explicitly with the `-i` / `--input` option, and the path where you want the file with the mocked functions to be generated as output using the `-m` / `--mock-output` option.

To generate expectation helper functions, pass the path where you want the files with the expectation helper functions to be generated as output using the `-e` / `--expect-output` option (additionally to or instead of the `-m` option).

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