
CppUMockGen Usage Manual
========================

## Introduction

CppUMockGen generates automatically mocks for functions and methods defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

## Getting Started

To generate a mock from a header file containing the functions and methods that you want to mock, just pass the path to the header file as input (first parameter or explicitly with the `-i` / `--input` option) and the path to the file where you want the mocked functions to be generated as output (second parameter or explicitly with the `-o` / `--output` option). If the output file is not specified, the mock is printed to the console.

CppUMock by default interprets header files with the extensions .hh, .hpp or .hxx as C\++. Other extensions are interpreted by default as C. To force the interpretation of a header file as C++ use the `-x` / `--cpp` option.

CppUMock, just as any C/C++ compiler, needs to know where to find other include files referenced by the input file in order to interpret it properly. Pass the paths to the necessary include directories by using the `-I` / `--include-path` option. Like with most compilers, you may use this option several times to indicate multiple include directories.

CppUMockGen deduces the data types to use with CppUMock from the actual function and method parameters and return types. If the API that you are mocking is well designed (e.g. pointers to non-const values are not used for input parameters), CppUMockGen will guess properly in most cases the correct types. Nevertheless, mocked data types can be overriden by using `-p` / `--param-override` options to override the type to use for specific functions or methods parameters and return types, and using `-t` / `--type-override` options to override the type to use for matching parameter or return types in any mocked function or method (see [Overriding Mocked Parameter and Return Types](#overriding-mocked-parameter-and-return-types) below).

## Command-Line Options

`CppUMockGen [OPTION...] [<input>] [<output>]`

| OPTION                        | Description                                   |
| -                             | -                                             |
| `-i, --input <input> `        | Input file                                    |
| `-o, --output <output>`       | Output file                                   |
| `-x, --cpp`                   | Force interpretation of the input file as C++ |
| `-u, --underlying-typedef`    | Use underlying typedef type                   |
| `-I, --include-path <path>`   | Include path                                  |
| `-p, --param-override <expr>` | Override parameter type                       |
| `-t, --type-override <expr>`  | Override generic type                         |
| `-h, --help`                  | Print help                                    |

## Mocked Parameter and Return Types

CppUMockGen deduces the <code>with<i>&lt;MockedType></i>Parameter</code>/<code>return<i>&lt;MockedType></i>Value</code> functions to call on `MockActualCall` objects from the C/C++ types according to the following table:

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

##### Handling of Typedefs

CppUMockGen uses the underlying type of typedefs to deduce the parameter or return function to call on `MockActualCall` objects.

If the resulting call for a parameter is `withParameterOfType` or `withOutputParameterOfType`, the typedef name will be used as the name of the type. If instead of using the typedef name you prefer to use the underlying class or struct name, use the `-u` / `--underlying-typedef` command line option.

## Overriding Mocked Parameter and Return Types

### Specific Mocked Parameter / Return Type Override

To override the <code>with<i>&lt;MockedType></i>Parameter</code>/<code>return<i>&lt;MockedType></i>Value</code> method to call on `MockActualCall` objects for a specific parameter or return value of a mocked function or method, an override specification can be passed on the command line using the `-p` / `--param-override` option.

##### Specific parameter type override

Specific parameter type override options take the form:

&ensp; <code><i>&lt;QualifiedFunctionName></i><b>'#'</b><i>&lt;ParameterName></i><b>'='</b><i>&lt;MockedType></i>[<b>'&#47;'</b><i>&lt;ArgExpr></i>]</code>

Where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully qualified name of a mocked function or method, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;ParameterName></i></code> is the name of the parameter.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the parameter, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output, Skip_.
  - When set to _Skip_ the corresponding method call on the `MockActualCall` object for the parameter will be skipped.
- <code><i>&lt;ArgExpr></i></code> is an optional argument expression that must contain th **'$'** character. If defined, it will be passed as the argument for the CppUMock actual call parameter function replacing **'$'** by the mocked function parameter name.

##### Specific return type override

Specific return type override options take the form:

&ensp; <code><i>&lt;QualifiedFunctionName></i><b>'@='</b><i>&lt;MockedType></i>[<b>'&#47;'</b><i>&lt;RetExpr></i>]</code>

Where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully qualified name of a mocked function or method, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the return value, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer_.
- <code><i>&lt;ArgExpr></i></code> is an optional argument expression that must contain the **'$'** character. If defined, it will be used as the return value of the mocked function or method, replacing **'$'** by the CppUMock actual call sequence.

### Generic Mocked Parameter / Return Type Override

To override the <code>with<i>&lt;MockedType></i>Parameter</code>/<code>return<i>&lt;MockedType></i>Value</code> method to call on `MockActualCall` objects for parameter or return values of a mocked functions or methods based on its data type, an override specification can be passed on the command line using the `-t` / `--type-override` option.

##### Generic parameter type override

Generic parameter type override options take the form:

&ensp; <code><b>'#'</b><i>&lt;ParameterType></i><b>'='</b><i>&lt;MockedType></i>[<b>'&#47;'</b><i>&lt;ArgExpr></i>]</code>

Where:
- <code><i>&lt;ParameterType></i></code> is a C/C++ data type.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the parameter, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output, Skip_.
  - When set to _Skip_ the corresponding method call on the `MockActualCall` object for the parameter will be skipped.
- <code><i>&lt;ArgExpr></i></code> is an optional argument expression that must contain the **'$'** character. If defined, it will be passed as the argument for the CppUMock actual call parameter function replacing **'$'** by the mocked function parameter name.

##### Generic return type override

Generic return type override options take the form:

&ensp; <code><b>'@'</b><i>&lt;ReturnType></i><b>'='</b><i>&lt;MockedType></i>[<b>'&#47;'</b><i>&lt;ArgExpr></i>]</code>

Where:
- <code><i>&lt;ReturnType></i></code> is a C/C++ data type.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the return value, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer_.
- <code><i>&lt;ArgExpr></i></code> is an optional argument expression that must contain the **'$'** character. If defined, it will be used as the return value of the mocked function or method, replacing **'$'** by the CppUMock actual call sequence.

#### Mocked Type Override Examples

###### Example 1: Specific parameter type override

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

If we want to override parameter `p1` to check just the pointed value, we can use the following option:

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

&ensp;
###### Example 2: Specific parameter and return type overrides

Assuming that we want to mock the following function:

```cpp
char* function2( const char *p );
```

If we want to override parameter `p` because it is not really a string and the return type because the real function is ill-designed and really just returns pointers to unmutable strings, we can use the following options:

```less
CppUMockGen <InputFilename> -p function2#p=ConstPointer -p function2@=String/static_cast<char*>($)
```

The generated mock would be as follows:

```cpp
char * function2(const char * p)
{
    return static_cast<char*>(mock().actualCall("function2").withConstPointerParameter("p", p).returnStringValue());
}
```

&ensp;
###### Example 3: Generic parameter and return type overrides

Assuming that we want to mock the following functions:

```cpp
std::string functionA( const std::string &p );
std::string functionB( const std::string &p1, std::string &p2 );
```

If we want to override all parameters of type `const std::string&` and `std::string` return types to be used as strings, we can use the following options:

```less
CppUMockGen <InputFilename> -t "#const std::string &=String/$.c_str()" -t @std::string=String
```

The generated mock would be as follows:

```cpp
std::string functionA(const std::string & p)
{
    return mock().actualCall("functionA").withStringParameter("p", p.c_str()).returnStringValue();
}

std::string functionB(const std::string & p1, std::string & p2)
{
    return mock().actualCall("functionB").withStringParameter("p1", p1.c_str()).withOutputParameterOfType("std::string", "p2", &p2).returnStringValue();
}
```

&ensp;
###### Example 4: Skipping a parameter

Assuming that we want to mock the following function:

```cpp
void foo( const char *p1, char *p2, int p3 );
```

If we want to skip parameter `p2`, we can use the following options:

```less
CppUMockGen <InputFilename> -p foo#p2=Skip
```

The generated mock would be as follows:

```cpp
void foo(const char * p1, char *, int p3)
{
    mock().actualCall("foo").withStringParameter("p1", p1).withIntParameter("p3", p3);
}
```
