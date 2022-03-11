
CppUMockGen Usage Manual
========================

## Contents

* [Introduction](#introduction)
* [Getting Started](#getting-started)
* [Command-Line Options](#command-line-options)
* [Input Files Processing](#input-files-processing)
* [Configuration Files](#configuration-files)
* [Mocked Parameter and Return Types](#mocked-parameter-and-return-types)
* [Ignoring Input Parameters in Expectations](#ignoring-input-parameters-in-expectations)
* [Overriding Mocked Parameter and Return Types](#overriding-mocked-parameter-and-return-types)
* [Examples](#examples)

## Introduction

CppUMockGen generates automatically mocks for member functions (a.k.a. class methods) and non-member functions (a.k.a. free functions) defined in C/C++ header files to avoid the burden of having to write these mocks manually when writing units tests using the CppUTest/CppUMock unit testing framework.

CppUMockGen additionally generates automatically expectation functions to improve and ease the declaration of expectations for functions in tests, making expectation declarations simpler, shorter, more readable and less error prone.

## Getting Started

### Mocks

#### What are Mocks?

Mocks are replacements for functions and methods that can simulate the real counterpart behavior during tests in a controlled way.

CppUTest/CppUMock offers a very powerful mechanism to implement mocks for being used in unit tests, however, defining and maintaining these mock implementations is a cumbersome, tedious and error prone task: the user has to keep the symbolic ("stringified") name of the functions and its parameters in sync with the actual C/C\++ signature and also with the actual names used in expectation declarations.

CppUMockGen can automatically generate for each mockable function its mocked implementation functions.

#### Generating Mocks

To generate mocks from a header file containing the functions that you want to mock, just use the `-m` / `--mock-output` option and pass the path to the header file as input in the first non-option argument (or explicitly with the `-i` / `--input` option).

CppUMockGen deduces the data types to use with CppUMock from the actual function parameters and return types (see [Mocked Parameter and Return Types](#mocked-parameter-and-return-types)). If the API that you are mocking is well designed (e.g. pointers to non-const values are not used for input parameters), CppUMockGen will guess properly in most cases the correct types. Nevertheless, mocked data types can be overridden by using `-t` / `--type-override` options to indicate the type of function parameters or function returns (see [Overriding Mocked Parameter and Return Types](#overriding-mocked-parameter-and-return-types) below).

#### Using Mocks

To use the generated mocks, you just need to setup your test generation scripts to compile and link the generated mock implementation files (e.g., <i>*_mock.cpp</i>) when generating the test executable.

### Expectations

#### What are Expectations?

CppUTest/CppUMock offers a very powerful mechanism to declare function call expectations in unit tests, however, defining and maintaining these expectation declarations has the same problems than defining and maintaining mocks: the user has to keep the symbolic ("stringified") name of the functions and its parameters in sync with the actual C/C\++ signature and also with the actual names used in mock definitions.

CppUMockGen can automatically generate for each mockable function (and also for mockable class constructors and destructors) its corresponding expectation functions (simply called expectations) that encapsulates the expectation cumbersome details into functions with a signature that closely resembles the mocked function's signature.

Expectation functions for member and non-member functions are named exactly as the expected function (e.g., the method `bar()` of class `foo` can be expected with `::expect::foo$::bar()`).

These expectations functions take as parameters an object (only for non-static methods), one parameter for each parameter in the mocked function, one additional parameter to pass the length of the data to copy for each non-typed output parameter, and finally one parameter to pass the return value (only for non-void return functions).

For each mockable function two expectation functions are declared, one for expecting a single call to the mocked function and another for expecting multiple calls (that additionally takes a first parameter to indicate the number of expected calls).

Expectation functions are declared inside root namespace `expect`, using the same namespace hierarchy than the mocked function but appending character **'$'** to each original namespace to avoid name clashes.

Additionally, for member functions (and also constructors and destructors), an innermost namespace hierarchy is declared according to the function class nesting hierarchy, also appending **'$'** to class names (e.g., expectations for member functions of class `foo` inside namespace `bar ` are declared inside namespace `expect::bar$::foo$`).

Expectation functions for constructors are named appending **'$ctor'** to class names (e.g., a constructor of class `foo` can be expected with `expect::foo$::foo$ctor()`). Expectation functions for constructors, like expectations for functions, take parameters for each parameter in the constructor, but don't take an object as parameter and do not have a parameter for a return value.

Expectation functions for destructors are named appending **'$dtor'** to class names (e.g., a destructor of class `foo` can be expected with `expect::foo$::foo$dtor()`). Expectation functions for destructors take an object as optional parameter.

Let's consider the signatures of expectations generated for different mocked function types:

- Non-member function with void return type
> ```cpp
> void function( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::function( T1 p1, ..., TN pN );
> void expect::function( unsigned int numCalls, T1 p1, ..., TN pN );
> ```

- Non-member function with non-void return type
> ```cpp
> TR functionWithReturn( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::functionWithReturn( T1 p1, ..., TN pN, TR returnValue );
> void expect::functionWithReturn( unsigned int numCalls, T1 p1, ..., TN pN, TR returnValue );
> ```

- Static method with void return type
> ```cpp
> void className::staticMethod( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::staticMethod( T1 p1, ..., TN pN );
> void expect::className$::staticMethod( unsigned int numCalls, T1 p1, ..., TN pN );
> ```

- Static method with non-void return type
> ```cpp
> TR className::staticMethodWithReturn( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::staticMethodWithReturn( T1 p1, ..., TN, pN, TR returnValue );
> void expect::className$::staticMethodWithReturn( unsigned int numCalls, T1 p1, ..., TN pN, TR returnValue );
> ```

- Non-static method with void return type
> ```cpp
> void className::method( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::method( className object, T1 p1, ..., TN pN );
> void expect::className$::method( unsigned int numCalls, className object, T1 p1, ..., TN pN );
> ```

- Non-static method with non-void return type
> ```cpp
> TR className::methodWithReturn( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::methodWithReturn( className object, T1 p1, ..., TN pN, TR returnValue );
> void expect::className$::methodWithReturn( unsigned int numCalls, className object, T1 p1, ..., TN pN, TR returnValue );
> ```

- Function with non-typed output parameters
> ```cpp
> void functionWithOutputParams( T1 *p1, ..., TN &pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::functionWithOutputParams( T1 *p1, size_t_ __sizeof_p1, ..., TN *pN, __sizeof_pN );
> void expect::functionWithOutputParams( numCalls, T1 *p1, size_t_ __sizeof_p1, ..., TN *pN, size_t_ __sizeof_pN );
> ```

- Class constructor
> ```cpp
> className::className( T1 p1, ..., TN pN );
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::className$ctor( T1 p1, ..., TN pN );
> void expect::className$::className$ctor( unsigned int numCalls, T1 p1, ..., TN pN );
> ```

- Class destructor
> ```cpp
> className::~className();
> ```
> &emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;&emsp;&#8659;
> ```cpp
> void expect::className$::className$dtor( className object );
> void expect::className$::className$dtor( unsigned int numCalls, className object );
> ```

To declare an expectation for a mocked function the user just has to call the expectation function (before calling the tested function) passing a pointer to the expected object on which a method is called (only for non-static methods), the expected parameters input and/or output values (or an indication to ignore an input parameter and accept any value), and the value to be returned by the mock when the expectation is fulfilled (if it has non-void return type).

> Simple expectations usage example:
> ```cpp
> using CppUMockGen::IgnoreParameter;
>
> TEST( TestSuite, Test )
> {
>    // Prepare
>    expect::functionWithReturn( 0xDEADBEEF, /* integer input parameter */
>                                "foo",      /* string input parameter */
>                                true );     /* return value */
>    expect::className$::method( 4,                    /* calls expected */
>                                IgnoreParameter::YES, /* object */
>                                "foo" );              /* string input parameter */
>
>    // Exercise
>    testedFunction();
>
>    // Verify
>    mock().checkExpectations();
> }
> ```

#### Generating Expectations

To generate expectations just use the `-e` / `--expect-output` option (additionally to or instead of the `-m` / `--mock-output` option) and pass the path to the mocked header file as input in the first non-option argument (or explicitly with the `-i` / `--input` option).

#### Using Expectations

To use the expectations, you must:
- In your test source file, include the generated expectation header files (e.g., <i>*_expect.hpp</i>).
- In your tests, call the expectation functions with the expected parameters and return values before calling the tested function.
- Setup your test generation scripts to find the header file _CppUMockGen.hpp_ (provided with CppUMockGen), i.e, add `-I <cppumockgen_home>/include` to compiler options, or copy the header file to your project.
- Setup your test generation scripts to compile and link the generated expectation implementation files (e.g., <i>*_expect.cpp</i>) when generating the test executable.

## Command-Line Options

`CppUMockGen [OPTION...] [<input>]`

| OPTION                                  | Description                                   |
| -                                       | -                                             |
| `-i, --input <input> `                  | Input file path                               |
| `-m, --mock-output [<mock-output>]`     | Mock output directory or file path            |
| `-e, --expect-output [<expect-output>]` | Expectation output directory or file path     |
| `-x, --cpp`                             | Force interpretation of the input file as C++ |
| `-s, --std`                             | Set language standard (c\+\+14, c\+\+17, etc.)|
| `-I, --include-path <path>`             | Include path                                  |
| `-B, --base-directory <path>`           | Base directory path                           |
| `-t, --type-override <expr>`            | Override generic type                         |
| `-f, --config-file <file>`              | Configuration file to be parsed for options   |
| `-v, --version`                         | Print version                                 |
| `-h, --help`                            | Print help                                    |

## Mocks and Expections Generation

#### Generating Mocks

To generate mocks use the `-m` / `--mock-output` option.

This mock output option accepts an optional argument to indicate the path where you want the file with the mocked functions to be generated as output.
 - When the output option argument is the path to an existing directory (or ends with a path separator), the output file name is derived from the input file name by replacing its extension with *"_mock.cpp"*, and it is generated in that directory.
 - When the output option argument is not specified, it is equivalent to passing the current directory.
 - When the output option argument is **'@'**, the mock is printed to the console.
 - In other cases the output option argument is considered the output file name (eventually adding the extension *".cpp"*).

#### Generating Expectations

To generate expectations use the `-e` / `--expect-output` option.

This expectation output option accepts an optional argument to indicate the path where you want the files with the expectation functions to be generated as output.
 - When the output option argument is the path to an existing directory (or ends with a path separator), the output file names are derived from the input file name by replacing its extension with *"_expect.cpp"* / *"_expect.hpp"*, and they are generated in that directory.
 - When an output option argument is not specified, it is equivalent to passing the current directory.
 - When the output option argument is **'@'**, the output is printed to the console.
 - In other cases the output file names are derived from the output option argument by adding or replacing the extensions *".cpp"* / *".hpp"*.

#### Generating Both Mocks and Expectations

Mocks and expectations can be generated at the same time by using both the `-m` / `--mock-output` option and the `-e` / `--expect-output` option.

If only a single output option argument is specified (e.g., `-m -e <output_dir>`), then the output file name(s) for the non-specified output option are derived from the specified one.
 - When the specified output option argument is the path to an existing directory (or ends with a path separator), it is equivalent to passing that path to both output options.
 - When the output option argument is **'@'**, both outputs are printed to the console.
 - In other cases it is equivalent to adding "_mock" or "_expect" to the specified filename and passing it to the other option.

> **Example:** `CppUMockGen -m -e <output_dir> -i MyClass.hpp` will generate the files "_&lt;output\_dir&gt;_/MyClass\_mock.cpp", "_&lt;output\_dir&gt;_/MyClass\_expect.cpp" and "_&lt;output\_dir&gt;_/MyClass\_expect.hpp".

> **Example:** `CppUMockGen -m MyClassMock.cpp -e -i MyClass.hpp` will generate the files "MyClassMock.cpp", "MyClassMock\_expect.cpp" and "MyClassMock\_expect.hpp".


## Input Files Processing

#### Input File

A single input file must be specified as the first non-option argument, or explicitly using the `-i` / `--input` option.

This input file must be a C or C++ header file containing the declarations of functions and/or methods.

#### C or C++?

CppUMockGen by default interprets input files with the extensions _.hh_, _.hpp_ or _.hxx_ as C\++. Other extensions are interpreted by default as C.

To force the interpretation of a header file as C++ use the `-x` / `--cpp` option.

#### Included Header Files Path

CppUMockGen, just as any C/C++ compiler, needs to know where to find other header files included by the input file in order to interpret it properly. Pass the paths to the necessary include directories by using the `-I` / `--include-path` option. Like with most compilers, you may use this option several times to indicate multiple include directories.

#### Base Directory

Mocks and expectations generated by CppUMockGen need to include the mocked header file. By default, the mocked header file will be referenced with a relative path from the directory where the mocks/expectations are generated.

> **Example:** When generating a mock for a file in _&lt;project-path&gt;/sources/MockedClass/MockedClass.h_ into _&lt;project-path&gt;/tests/mocks/_, the mocked header will be included as `#include "../../sources/MockedClass/MockedClass.h"`.

This behavior may be overriden (e.g., to generate files that can be moved in the directory structure) using the `-B` / `--base-directory` option to set a base directory to reference included mocked headers (e.g., set it to a directory that is in the include path when compiling tests).

> **Example:** When generating a mock for a file in _&lt;project-path&gt;/sources/MockedClass/MockedClass.h_ into _&lt;project-path&gt;/tests/mocks/_ using the option `-B <project-path>/sources`, the mocked header will be included as `#include "MockedClass/MockedClass.h"`.

#### Language

CppUMockGen parses C code according to the `gnu11` language standard, and parses C\+\+ code according to the `gnu++14` language standard. To indicate a different standard, use the `-s` / `--std` and pass a supported language standard. The list of supported language standards can be found in the [Clang compiler command line help](https://clang.llvm.org/docs/CommandGuide/clang.html#cmdoption-std).

## Configuration Files

Complex mocks will require a lot of override options, and many of them can be reused to generate mocks for other files. To facilitate reusing options, they can be stored in a text file that can be loaded using the `-f` / `--config-file` option.

Options in a configuration file are stored as arguments similar as they are passed on the command line, separating them using white-spaces and/or newlines, and using double quotes (`"`) to delimit arguments that have white-spaces. Double quotes inside an argument can be escaped preceding it with a backslash (e.g. `-t "const std::string &=String~($+\"#\").c_str()"`).

Multiple configuration files can be specified on the command line. Configuration files may even include other configuration files using the `-f` / `--config-file` option. If a configuration file includes another configuration file using a relative path, the path is resolved relative to the including file location (i.e. not relative to the directory from which CppUMockGen was executed).

## Mocked Parameter and Return Types

CppUMockGen deduces the <code>with<i>&lt;MockedType></i>Parameter</code>/<code>return<i>&lt;MockedType></i>Value</code> methods to call on `MockActualCall` objects from the C/C++ types according to the following table:

| C/C++ type                                                        | CppUMock parameter function | CppUMock return function |
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
| <code>const <i>&lt;PrimitiveType></i> &ast;</code> <br> <code>const <i>&lt;PrimitiveType></i> &</code> | `withConstPointerParameter` | `returnConstPointerValue` |
| <code><i>&lt;PrimitiveType></i> &ast;</code> <br> <code><i>&lt;PrimitiveType></i> &</code> | `withOutputParameter` | `returnPointerValue` |
| <code><i>&lt;PrimitiveType></i> &&</code>                         | _Same as_ <code><i>&lt;PrimitiveType></i></code> | `returnPointerValue` |
| `const class *` <br> `const struct *` <br> `const class &` <br> `const struct &` | `withParameterOfType` | `returnConstPointerValue` |
| `class *` <br> `struct *` <br> `class &` <br> `struct &` <br> `class &&` <br> `struct &&` | `withOutputParameterOfType` | `returnPointerValue` |
| `void *` <br> <code><i>&lt;AnyType></i> &ast; &ast;</code> <br> <code><i>&lt;AnyType></i> &ast; &</code>    | `withPointerParameter` | `returnPointerValue` |

> **Note:** _&lt;PrimitiveType>_ is any of: `bool`, `char`, `short`, `int`, `wchar_t`, `unsigned char`, `unsigned short`, `unsigned int`, `char16_t`, `long`, `unsigned long`, `char32_t`, `float`, `double`, `enum`, `enum class`.

Expectations generation works in the same way than mock generation, but using instead the equivalent methods for calling `MockExpectedCall` objects.

#### Handling of Classes and Structs

When a parameter is based on a non-primitive type (i.e., `class` or `struct`) either passed by copy, by pointer or by reference, CppUMockGen generates calls to `withParameterOfType` or `withOutputParameterOfType` passing the fully-qualified name of the parameter's type in the call's `typeName` argument.

CppUMock needs comparators (implementations of the interface `MockNamedValueComparator`) and copiers (implementations of the interface `MockNamedValueCopier`) to be installed at test time for all the types used in respectively `withParameterOfType` and `withOutputParameterOfType` calls.

CppUMockGen does not generate comparators or copiers, so users will have to provide suitable implementations (see CppUMock manual section regarding ['Objects as Parameters'](http://cpputest.github.io/mocking_manual.html#objects_as_parameters) and ['Output Parameters Using Objects'](http://cpputest.github.io/mocking_manual.html#output_parameters_using_objects)).

#### Handling of Typedefs

CppUMockGen recursively deduces the underlying type of typedefs to determine the parameter or return method to call on `MockActualCall` objects.

If the resulting call for a parameter is `withParameterOfType` or `withOutputParameterOfType`, the typedef name will be used as the name of the type. If instead of using the typedef name you prefer to use the underlying class or struct name, use the `-u` / `--underlying-typedef` command line option.

## Ignoring Input Parameters in Expectations

When using expectations, you may selectively indicate that an input parameter can take any value by passing the value CppUMockGen::IgnoreParameter::YES as the argument to the parameter in the expectation function call.

You may also indicate that a method can be invoked on any object instance by passing the value CppUMockGen::IgnoreParameter::YES as the argument to the object parameter in the expectation function call.

## Overriding Mocked Parameter and Return Types

To override the <code>with<i>&lt;MockedType></i>Parameter</code>/<code>return<i>&lt;MockedType></i>Value</code> method used in mocks to call on `MockActualCall` objects, override specifications can be passed on the command line.

At the same time override specifications also define in expectations the <code>with<i>&lt;MockedType></i>Parameter</code> method to call and the specific <code>andReturnValue</code> override method to call on `MockExpectedCall` objects.

The type override specifications can be defined for specific parameters or return values of specific mocked functions, or can be generic (based on data type).

Function-specific overrides take preference over generic overrides: If both a function-specific and a generic override match a function parameter or return, the function-specific override will be applied.

A function-specific or generic type override specification is passed on the command line using the `-t` / `--type-override` option. The specification format is described in the following sections.

> **Note:** Use the same override specifications when generating the mocks and expectations for a header file, otherwise the mock and the expectations will probably not be compatible and will break the tests. To ensure this it is recommended to generate both the mock and expectations in a single run by passing both options `-m` and `-e` on the command line.

#### Function-Specific Parameter Type Override

Function-specific parameter type override options take the form:

&ensp; <code><i>&lt;QualifiedFunctionName></i><b>'#'</b><i>&lt;ParameterName></i><b>'='</b><i>&lt;MockedType></i>[<b>'~'</b><i>&lt;ArgExpr></i>]</code>

Where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully-qualified name of a mocked function, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;ParameterName></i></code> is the name of the parameter.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the parameter, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output, Skip_, or an <code>InputOfType</code> or <code>OutputOfType</code> option.
  - When set to _Skip_ the corresponding method call on the `MockActualCall` and  `MockExpectedCall` objects for the parameter will be skipped, and the parameter will not appear in the signature of expectation functions.
  - An <code>InputOfType</code> option takes the form <b>'InputOfType:'</b><code><i>&lt;DeclaredType></i>[<b>'&lt;'</b><i>&lt;ExpectationType></i>]</code>, where:
    - <code><i>&lt;DeclaredType></i></code> indicates the type name to be passed to the <code>withParameterOfType</code> method of <code>MockActualCall</code> and <code>MockExpectedCall</code>.
    - <code><i>&lt;ExpectationType></i></code> optionally indicates a different parameter's type to use in expectation functions instead of the original type.
  - An <code>OutputOfType</code> option takes the form <b>'OutputOfType:'</b><code><i>&lt;DeclaredType></i>[<b>'&lt;'</b><i>&lt;ExpectationType></i>]</code>, where:
    - <code><i>&lt;DeclaredType></i></code> indicates the type name to be passed to the <code>withOutputParameterOfType</code> method of <code>MockActualCall</code> and to the <code>withOutputParameterOfTypeReturning</code> method of <code>MockExpectedCall</code>.
    - <code><i>&lt;ExpectationType></i></code> optionally indicates a different parameter's type to use in expectation functions instead of the original type.
- <code><i>&lt;ArgExpr></i></code> is an optional C++ expression that must contain the **'$'** placeholder character. If defined, it will be passed as the argument for the CppUMock actual call parameter method, replacing **'$'** by the mocked function parameter name.

#### Function-Specific Mocked Return Type Override

Function-specific return type override options take the form:

&ensp; <code><i>&lt;QualifiedFunctionName></i><b>'@='</b><i>&lt;MockedType></i>[<b>'~'</b><i>&lt;RetExpr></i>]</code>

Where:
- <code><i>&lt;QualifiedFunctionName></i></code> is the fully-qualified name of a mocked function, including namespace (e.g. `namespace1::class1::method1`).
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the return value, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer_.
- <code><i>&lt;RetExpr></i></code> is an optional C++ expression that must contain the **'$'** placeholder character. If defined, it will be used as the return value of the mocked function, replacing **'$'** by the CppUMock actual call sequence.

#### Generic Mocked Parameter Type Override

Generic parameter type override options take the form:

&ensp; <code><b>'#'</b><i>&lt;ParameterType></i><b>'='</b><i>&lt;MockedType></i>[<b>'~'</b><i>&lt;ArgExpr></i>]</code>

Where:
- <code><i>&lt;ParameterType></i></code> is a C/C++ data type.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the parameter, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer, Output, Skip_, or an <code>InputOfType</code> or <code>OutputOfType</code> option.
  - When set to _Skip_ the corresponding method call on the `MockActualCall` and  `MockExpectedCall` objects for the parameter will be skipped, and the parameter will not appear in the signature of expectation functions.
  - An <code>InputOfType</code> option takes the form <b>'InputOfType:'</b><code><i>&lt;DeclaredType></i>[<b>'&lt;'</b><i>&lt;ExpectationType></i>]</code>, where:
    - <code><i>&lt;DeclaredType></i></code> indicates the type name to be passed to the <code>withParameterOfType</code> method of <code>MockActualCall</code> and <code>MockExpectedCall</code>.
    - <code><i>&lt;ExpectationType></i></code> optionally indicates an override type to use as the argument of expectation functions.
  - An <code>OutputOfType</code> option takes the form <b>'OutputOfType:'</b><code><i>&lt;DeclaredType></i>[<b>'&lt;'</b><i>&lt;ExpectationType></i>]</code>, where:
    - <code><i>&lt;DeclaredType></i></code> indicates the type name to be passed to the <code>withOutputParameterOfType</code> method of <code>MockActualCall</code> and to the <code>withOutputParameterOfTypeReturning</code> method of <code>MockExpectedCall</code>.
    - <code><i>&lt;ExpectationType></i></code> optionally indicates a different parameter's type to use in expectation functions instead of the original type.
- <code><i>&lt;ArgExpr></i></code> is an optional C++ expression that must contain the **'$'** placeholder character. If defined, it will be passed as the argument for the CppUMock actual call parameter method, replacing **'$'** by the mocked function parameter name.

#### Generic Mocked Return Type Override

Generic return type override options take the form:

&ensp; <code><b>'@'</b><i>&lt;ReturnType></i><b>'='</b><i>&lt;MockedType></i>[<b>'~'</b><i>&lt;RetExpr></i>]</code>

Where:
- <code><i>&lt;ReturnType></i></code> is a C/C++ data type.
- <code><i>&lt;MockedType></i></code> indicates the CppUMock type to use for the return value, admitted values are: _Bool, Int, UnsignedInt, LongInt, UnsignedLongInt, Double, String, Pointer, ConstPointer_.
- <code><i>&lt;RetExpr></i></code> is an optional C++ expression that must contain the **'$'** character. If defined, it will be used as the return value of the mocked function, replacing **'$'** by the CppUMock actual call sequence.

## Examples

### Example 1: Simple generation

Assuming that we want to mock the following class:

```cpp
namespace namespace1
{
    int Function1( int p1, const char* p2, int *p3 );
}
```

To generate the mock and expectations we can use the following command:

```powershell
CppUMockGen -i <InputFilename> -m -e
```

Assuming that the tested function should call `Function1` only once, passing 25 in `p1`, the C-string "foo" in `p2`, and then the mocked function should fill `p3` with an array of 3 integers and return `true`, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test1 )
{
    // Prepare
    SomeClass* object = new SomeClass( 42 );
    int output[] = { 5, 25, 42 };
    expect::namespace1$::Function1( 25, "foo", output, sizeof(output), 3 );

    // Exercise
    bool ret = FunctionToTest1( object );

    // Verify
    CHECK_TRUE( ret );
    mock().checkExpectations();

    // Cleanup
    delete object;
}
```

&ensp;
### Example 2: Function-specific parameter type override

Assuming that we want to mock the following class, which has method which gets an input string but it is ill-designed (i.e., the parameter should had been declared as a reference to a _const_ string instead of just as a reference to a _non-const_ string):

```cpp
namespace namespace1
{
    class Class2
    {
    public:
        int Method1( std::string &p1 );
    };
}
```

CppUMockGen will determine that `p1` is an output parameter, since by default parameters which types are pointers or references to _non-const_ primitive, class or struct types are considered output parameters.

Fortunately, we can override parameter `p1` to be used as an input string using the following command:

```powershell
CppUMockGen <InputFilename> -m -e -t "namespace1::Class2::Method1#p1=String~$.c_str()"
```

Assuming that the tested function should call `Class2::Method1` only once on the same object that was passed to it, passing a string with value "foo" in `p1`, and then the mocked method should return 0, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test2 )
{
    // Prepare
    Class2 *object = new Class2();
    const char* expectedText = "foo";
    expect::namespace1$::Class2$::Method1( object, expectedText, 0 );

    // Exercise
    FunctionToTest2( object );

    // Verify
    mock().checkExpectations();

    // Cleanup
    delete object;
}
```

&ensp;
### Example 3: Function-specific parameter and return type overrides

Assuming that we want to mock the following function:

```cpp
char* Function3( const char *p1 );
```

CppUMockGen will determine that `p1` is an input string, since by default parameters which types are pointers to _const_ characters are considered input strings.

If we are not interested in the contents of the string passed in `p1` and rather we want to verify the actual value of the pointer, and also want to override the return type because the real function is ill-designed and really just returns pointers to unmutable strings, we can use the following command:

```powershell
CppUMockGen <InputFilename> -m -e -t "Function3#p1=ConstPointer" -t "Function3@=String~const_cast<char*>($)"
```

Assuming that the tested function should call `Function3` 4 times with the same pointer in `p1` that was passed to the tested function, and then the mocked function should return "OK" the first time and "NOK" the other times, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test3 )
{
    // Prepare
    const char* param = "foo";
    const char* returnValue1 = "OK";
    const char* returnValue2 = "NOK";
    expect::Function3( param, returnValue1 );
    expect::Function3( 3, param, returnValue2 );

    // Exercise
    FunctionToTest3( param );

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 4: Generic parameter and return type overrides

Assuming that we want to mock the following functions:

```cpp
std::string Function4A( const std::string &p1A );
std::string Function4B( const std::string &p1B, unsigned int p2B );
```

We can avoid having to install a comparator for `std::string` and simplify handling `std::string` return types by overriding all parameters of type `const std::string&` and `std::string` return types to be used as simple C strings, using the following command:

```powershell
CppUMockGen <InputFilename> -m -e -t "#const std::string &=String~$.c_str()" -t "@std::string=String"
```

Assuming that the tested function should:

1. Call `Function4A` once, passing the C string "foo" in `p1A`, and then the mocked function should return "foo-bar".
2. Call `Function4B` once, passing the C string "foo-bar" in `p1B` and 0 in `p2B`, and then the mocked function should return "foo-bar-moo".

We can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test4 )
{
    // Prepare
    const char* param1 = "foo";
    const char* ret1 = "foo-bar";
    const char* ret2 = "foo-bar-moo";
    expect::Function4A( param1, ret1 );
    expect::Function4B( ret1, 0, ret2 );

    // Exercise
    int ret = FunctionToTest4( param1 );

    // Verify
    CHECK_EQUAL( 0, ret );
    mock().checkExpectations();
}
```

&ensp;
### Example 5: Skipping a parameter (at generation time)

Assuming that we want to mock the following function:

```cpp
void Function5( const char *p1, char *p2, int p3 );
```

If we want to always ignore parameter `p2`, we can use the following command:

```powershell
CppUMockGen <InputFilename> -m -e -t "Function5#p2=Skip"
```

Assuming that the tested function should call `Function5` once, passing "foo" in `p1` and 10 in `p3`, and ignoring the value in `p2`, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test5 )
{
    // Prepare
    const char* param = "foo";
    expect::Function5( param, 10 );

    // Exercise
    FunctionToTest5();

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 6: Ignoring a parameter (at test time)

Assuming that we want to mock the following class:

```cpp
namespace namespace1
{
    class Class6
    {
    public:
        void Method1( const char* p1, int p2 );
    };
}
```

To generate the mock and expectations we can use the following command:

```powershell
CppUMockGen <InputFilename> -m -e
```

Assuming that the tested function should call `Class6::Method1` twice, on any object (it is created by the tested function, so we don't know it), passing a string with value "foo" in `p1`, and ignoring the value of `p2` (so it can take any value), we can use the generated mock and expectations in an unit test like this:

```cpp
using CppUMockGen::IgnoreParameter;

TEST( TestSuite, Test6 )
{
    // Prepare
    expect::namespace1$::Class6$::Method1( 2, IgnoreParameter::YES, "foo", IgnoreParameter::YES );

    // Exercise
    FunctionToTest6();

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 7: Typed input parameter with symmetrical comparator

Assuming that we want to mock the following function:

```cpp
void Function7( const std::vector<std::string> &p1 );
```

To generate the mock and expectations we can use the following command:

```powershell
CppUMockGen <InputFilename> -m -e
```

CppUMock needs a comparator class to compare the actual and the expected values of `p1`. We can use the following implementation:

```cpp
class StdVectorOfStringsComparator : public MockNamedValueComparator
{
public:
    bool isEqual(const void* object1, const void* object2)
    {
        const std::vector<std::string> *o1 = (const std::vector<std::string>*) object1;
        const std::vector<std::string> *o2 = (const std::vector<std::string>*) object2;

        return (*o1) == (*o2);
    }

    SimpleString valueToString(const void* object)
    {
        SimpleString ret;
        const std::vector<std::string> *o = (const std::vector<std::string>*) object;
        for( unsigned int i = 0; i < o->size(); i++ )
        {
            ret += StringFromFormat("<%u>%s\n", i, (*o)[i].c_str() );
        }
        return ret;
    }
};
```

Assuming that the tested function should call `function7` once, passing a vector of strings containing the values "foo" and "bar" in `p1`, we can use the generated mock and expectations in an unit test like this:

```cpp
StdVectorOfStringsComparator stdVectorOfStringsComparator;

TEST( TestSuite, Test7 )
{
    // Prepare
    mock().installComparator( "std::vector<std::string>", stdVectorOfStringsComparator );

    std::vector<std::string> expectedParam = { "foo", "bar" };

    expect::Function7( expectedParam );

    // Exercise
    FunctionToTest7();

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 8: Typed output parameter with asymmetrical copier

Assuming that we want to mock the following function:

```cpp
void Function8( std::ostream &p1 );
```

Function `Function8` should write something into the passed output stream. Therefore, since the passed object is modified, `p1` is considered as an output parameter.

As it is cumbersome (if not even impossible) to copy an output stream into another, a symmetrical copier (where both copier parameters are of the same type) is not feasible. In this case we can define an asymmetrical CppUMock copier (where copier parameters are of different types) that writes a string into the output stream:

```cpp
class StdOstreamCopier : public MockNamedValueCopier
{
public:
    virtual void copy(void* out, const void* in)
    {
        *(std::ostream*)out << (const char*)in;
    }
};
```

CppUMockGen default behavior with typed output parameters is to expect a symmetrical copier. To indicate to CppUMockGen to use an asymmetrical copier for all references to `std::ostream`, we can use the following command:

```powershell
CppUMockGen <InputFilename> -m -e -t "#std::ostream &=OutputOfType:std::ostream<char~&$"
```

Now the expectation functions get a C string instead of a reference to an output stream.

Assuming that the tested function should call `Function8` once, passing an output stream in `p1`, then the mocked function should write "foo" on the stream, and finally the tested function should return `true`, we can use the generated mock and expectations in an unit test like this:


```cpp
StdOstreamCopier stdOstreamCopier;

TEST( TestSuite, Test8 )
{
    // Prepare
    mock().installCopier( "std::ostream", stdOstreamCopier );

    expect::Function8( "foo" );

    // Exercise
    bool ret = FunctionToTest8();

    // Verify
    CHECK_EQUAL( true, ret );
    mock().checkExpectations();
}
```

&ensp;
### Example 9: Non-typed struct output parameter

Assuming that we want to mock the following function:

```cpp
struct Struct9
{
    int a;
    int b;
};

void Function9( Struct9 *p1 );
```

CppUMockGen will determine that `p1` is a typed output parameter, since by default parameters which types are pointers or references to _non-const_ class or struct types are considered typed output parameters. This means that we need a copier to be installed in order to copy `Struct9` objects.

However, taking in account that `Struct9` is a simple data type (also known as _POD_ or _Plain Old Data_ type) that can be copied just by copying memory, we can indicate to CppUMockGen to override `p1` as a non-typed output using the following command:

```powershell
CppUMockGen -i <InputFilename> -m -e -t "Function9#p1=Output"
```

Assuming that the tested function should call `Function9` once, passing a pointer to a `Struct9` struct in `p1`, and then the mocked function should initialize that struct, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test9 )
{
    // Prepare
    Struct9 output = { 2344, 4324 };

    expect::Function9( output, sizeof(output) );

    // Exercise
    FunctionToTest9();

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 10: Non-typed string output parameter

Assuming that we want to mock the following function:

```cpp
void Function10( char *str, int maxStrSize );
```

To generate the mock and expectations we can use the following command:

```powershell
CppUMockGen -i <InputFilename> -m -e
```

CppUMockGen will determine that `str` is an non-typed output parameter, since by default parameters which types are pointers or references to _non-const_ primitive types are considered non-typed output parameters.

Assuming that the tested function should call `Function10` once, passing a buffer to receive a string in `str`, and the size of that buffer in `maxStrSize` set to 100, and then the mocked function should write "foo" on the passed buffer, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test10 )
{
    // Prepare
    const char output[] = "foo";

    expect::Function10( output, sizeof(output), 100 );

    // Exercise
    FunctionToTest10();

    // Verify
    mock().checkExpectations();
}
```

&ensp;
### Example 11: Non-typed array output parameter

Assuming that we want to mock the following function, that gets an array and its number of elements as parameters:

```cpp
void Function11( int *array, unsigned int arrayLen );
```

To generate the mock and expectations we can use the following command:

```powershell
CppUMockGen -i <InputFilename> -m -e
```

CppUMockGen will determine that `array` is an non-typed output parameter, since by default parameters which types are pointers or references to _non-const_ primitive types are considered non-typed output parameters.

Assuming that the tested function should call `Function11` once, passing the array to be initialized in `array`, and the number of elements of that array in `arrayLen` set to 5, and then the mocked function should initialize the array, we can use the generated mock and expectations in an unit test like this:

```cpp
TEST( TestSuite, Test11 )
{
    // Prepare
    const int output[] = { 1, 2, 3, 4, -1 };

    expect::Function11( output, sizeof(output), 5 );

    // Exercise
    FunctionToTest11();

    // Verify
    mock().checkExpectations();

    // Cleanup
}
```
