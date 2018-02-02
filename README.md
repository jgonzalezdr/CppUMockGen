
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

##### Mocked Parameter / Return  Type Override

To override the<code>with<i>&lt;Type></i>Parameter</code>/<code>return<i>&lt;Type></i>Value</code>function to call on<code>MockActualCall</code>objects deduced by CppUMockGen, a parameter / return type override specification can be passed on the command line using the `-p` / `--param-override` option.

Parameter type override options take the form <code><i>&lt;QualifiedFunctionName></i><b>'#'</b><i>&lt;ParameterName></i><b>'='</b><i>&lt;Type></i>[<b>'&#47;'</b><i>&lt;ArgExpr></i>]</code>, where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully qualified name of a function, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;ParameterName></i></code> is the name of the parameter.
- <code><i>&lt;Type></i></code> indicates the function call to use for the parameter, admitted values are: Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output.
- <code><i>&lt;ArgExpr></i></code> is an optional argument expression. If it exists, it must contain the character<code><b>'$'</b></code>, and it will be passed as the argument for the CppUMock actual call parameter function (i.e.<code>with<i>&lt;Type></i>Parameter</code>) replacing<code><b>'$'</b></code>by the mocked function parameter name.

Return type override options take the form <code><i>&lt;QualifiedFunctionName></i><b>'@='</b><i>&lt;Type></i>[<b>'&#47;'</b><i>&lt;RetExpr></i>]</code>, where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully qualified name of a function, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;Type></i></code> indicates the function call to use for the return value, admitted values are: Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer.
- <code><i>&lt;RetExpr></i></code> is an optional return expression. If it exists, it must contain the character<code><b>'$'</b></code>, and it will be passed as the return value of the mocked function replacing<code><b>'$'</b></code>by the generated CppUMock actual call sequence.

###### Example 1: Parameter Type Override

Assuming that we want to mock the following class:

```cpp
namespace namespace1
{
    class class1
    {
        int method1( int *p1 );
    };
}
```

If we want to override parameter `p1` of type `int*`to check just the pointed value, we can use the following option:

```less
CppUMockGen <InputFilename> -p namespace1::class1::method1#p1=Int/*$
```

The generated mock would be as follows:

```cpp
int namespace1::class1::method1(int * p1)
{
    return mock().actualCall("namespace1::class1::method1").withIntParameter("p1", *p1).returnIntValue();
}
```


###### Example 2: Parameter and Return Type Override

Assuming that we want to mock the following function:

```cpp
std::string function2( const std::string &param1 );
```

If we want to override parameter `param1` of type `const std::string&` and the return type `std::string` to be used both as strings, we can use the following options:

```less
CppUMockGen <InputFilename> -p function2#param1=String/$.c_str() -p function2@=String
```

The generated mock would be as follows:

```cpp
std::string function2(const std::string & param1)
{
    return mock().actualCall("function2").withStringParameter("param1", param1.c_str()).returnStringValue();
}
```

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