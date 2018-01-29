
CppUMockGen
===========
Mock generator for [CppUTest/CppUMock](http://cpputest.github.io/).

[![Build status](https://ci.appveyor.com/api/projects/status/3ch50mcu29pgv5st/branch/master?svg=true)](https://ci.appveyor.com/project/jgonzalezdr/cppumockgen/branch/master)
[![codecov](https://codecov.io/gh/jgonzalezdr/CppUMockGen/branch/master/graph/badge.svg)](https://codecov.io/gh/jgonzalezdr/CppUMockGen)

## What is CppUMockGen?

CppUMockGen generates automatically mocks for functions and methods defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

## Download Binaries

* [GitHub Releases](https://github.com/jgonzalezdr/CppUMockGen/releases)
* [Last successful Windows build of the master branch](https://ci.appveyor.com/api/projects/jgonzalezdr/CppUMockGen/artifacts/CppUMockGen.exe?branch=master&job=Environment:%20Platform=MinGW64,%20PlatformToolset=6.3.0,%20Configuration=Release,%20Test=False,%20PublishArtifacts=True,%20APPVEYOR_BUILD_WORKER_IMAGE=Visual%20Studio%202015)

## Getting Started

### Usage

`CppUMockGen [OPTION...] [<input>] [<output>]`

| OPTION                        | Description                                   |
| -                             | -                                             |
| `-i, --input <input> `        | Input file                                    |
| `-o, --output <output>`       | Output file                                   |
| `-x, --cpp`                   | Force interpretation of the input file as C++ |
| `-u, --underlying-typedef`    | Use underlying typedef type                   |
| `-I, --include-path <path>`   | Include path                                  |
| `-p, --param-override <expr>` | Override parameter type                       |
| `-h, --help`                  | Print help                                    |

To generate a mock from a header file containing the functions and methods that you want to mock, just pass the path to the header file as input (first parameter or explicitly with the `-i` / `--input` option) and the path to the file where you want the mocked functions to be generated as output (second parameter or explicitly with the `-o` / `--output` option). If the output file is not specified, the mock is printed to the console.

CppUMock by default interprets header files with the extensions .hh, .hpp or .hxx as C\++. Other extensions are interpreted by default as C. To force the interpretation of a header file as C++ use the `-x` / `--cpp` option.

CppUMock, just as any C/C++ compiler, needs to know where to find other include files referenced by the input file in order to interpret it properly. Pass the paths to the necessary include directories by using the `-I` / `--include-path` option. Like with most compilers, you may use this option several times to indicate multiple include directories.

#### Mocked Parameter and Return Types

CppUMockGen deduces the `with<Type>Parameter` or `return<Type>Value` function to call on `MockActualCall` objects from the C/C++ types according to the following table:

| C/C++ type                                                        | CppUTest parameter function | CppUTest return function |
| -                                                                 | - | - |
| `bool`                                                            | `withBoolParameter`  | `returnBoolValue` |
| `char` <br> `short` <br> `int` <br> `wchar_t`                     | `withIntParameter` | `returnIntValue` |
| `unsigned char` <br> `unsigned short` <br> `unsigned int` <br> `char16_t` | `withUnsignedIntParameter` | `returnUnsignedIntValue` |
| `long`                                                            | `withLongIntParameter` | `returnLongIntValue` |
| `unsigned long` <br> `char32_t`                                   | `withUnsignedLongIntParameter` |  `returnUnsignedLongIntValue` |
| `float` <br> `double`                                             | `withDoubleParameter` | `returnDoubleValue` |
| `enum` <br> `enum class`                                          | `withIntParameter` | `returnIntValue` |
| `class` <br> `struct`                                             | `withParameterOfType` | `returnConstPointerValue` |
| `const char *`                                                    | `withStringParameter` | `returnStringValue` |
| `const <PrimitiveType> *` <br> `const <PrimitiveType> &`          | `withConstPointerParameter` | `returnConstPointerValue` |
| `<PrimitiveType> *` <br> `<PrimitiveType> &`                      | `withOutputParameter` | `returnPointerValue` |
| `<PrimitiveType> &&`                                              | _Same as_ `<PrimitiveType>`  | `returnPointerValue` |
| `const class *` <br> `const struct *` <br> `const class &` <br> `const struct &` | `withParameterOfType` | `returnConstPointerValue` |
| `class *` <br> `struct *` <br> `class &` <br> `struct &` <br> `class &&` <br> `struct &&` | `withOutputParameterOfType` | `returnPointerValue` |
| `void *` <br> `<PrimitiveType> * *` <br> `<PrimitiveType> * &`    | `withPointerParameter` | `returnPointerValue` |

###### Handling of Typedefs

CppUMockGen uses the underlying type of typedefs to deduce the parameter or return function to call on `MockActualCall` objects.

If the resulting call for a parameter is `withParameterOfType` or `withOutputParameterOfType`, the typedef name will be used as the name of the type. If instead of using the typedef name you prefer to use the underlying class or struct name, use the `-u` / `--underlying-typedef` command line option.

##### Mocked Parameter Type Override

To override the  `with<Type>Parameter` function to call on `MockActualCall` objects deduced by CppUMockGen, a parameter override specification can be passed on the command line using the `-p` / `--param-override` option.

Parameter override options take the form `<Key>=<OverrideSpecification>`, where:
- `<Key>` is a parameter identifier in the form `<QualifiedFunctionName>#<ParameterName>`, where:
  - `<QualifiedFunctionName>` is the fully qualified name of a function, including namespace (e.g. `namespace1::class1::method1`).
  - `<ParameterName>` is the name of the parameter.
- `<OverrideSpecification>` is an override specification, in the form `<Type>[/<ArgExpr>]`, where:
  - `<Type>` indicates the function call to use for the parameter, admitted values are: Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output.
  -  `<ArgExpr>` is an optional argument expression. If it exists, it must contain the character '$', and the expression used to pass the parameter of the mocked function to the CppUMock function is generated by replacing '$' in the argument expression by the parameter name.

###### Examples

Override a parameter of type `int*`to check just the pointed value:

    namespace1::class1::method1#p1=Int/*$

Override a parameter of type `std::string&`to use as a string:

    function2#param1=String/$.c_str()

## Build from Source

### Requirements

- [cmake](https://cmake.org/) (tested with v3.7.1)
- [clang](http://releases.llvm.org/download.html) (tested with v5.0.1)
- A C/C++ compiler, either:
  - [MinGW-w64](https://sourceforge.net/projects/mingw-w64/) (tested with x86_64-6.4.0-posix-seh-rt_v5-rev0)
  - [Microsoft Visual Studio](https://www.visualstudio.com/es/downloads/) (tested with Visual Studio Community 2015 and 2017)
- [CppUTest](http://cpputest.github.io/) [Optional, not needed if tests are disabled] (tested with v3.7.1 and v3.8)
- [LCOV](https://github.com/jgonzalezdr/lcov/releases) [Optional, needed if tests and coverage are enabled using GCC/MinGW] (tested with v1.11.1w)
- [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) [Optional, needed if tests and coverage are enabled using Visual Studio 2017] (tested with v0.9.6.1)