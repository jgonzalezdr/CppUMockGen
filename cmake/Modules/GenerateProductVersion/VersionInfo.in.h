/*
The MIT License (MIT)

Copyright (c) 2015, by [halex2005](mailto:akharlov@gmail.com)
Copyright (c) 2018, by Jesus Gonzalez <jgonzalez@gdr-sistemas.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#define PRODUCT_VERSION_MAJOR @PRODUCT_VERSION_MAJOR@
#define PRODUCT_VERSION_MINOR @PRODUCT_VERSION_MINOR@
#define PRODUCT_VERSION_PATCH @PRODUCT_VERSION_PATCH@
#define PRODUCT_VERSION_BUILD @PRODUCT_VERSION_BUILD@

#ifndef __TO_STRING
#define __TO_STRING_IMPL(x) #x
#define __TO_STRING(x) __TO_STRING_IMPL(x)
#endif

#define PRODUCT_VERSION_MAJOR_MINOR_STR        __TO_STRING(PRODUCT_VERSION_MAJOR) "." __TO_STRING(PRODUCT_VERSION_MINOR)
#define PRODUCT_VERSION_MAJOR_MINOR_PATCH_STR  PRODUCT_VERSION_MAJOR_MINOR_STR "." __TO_STRING(PRODUCT_VERSION_PATCH)
#define PRODUCT_VERSION_FULL_STR               PRODUCT_VERSION_MAJOR_MINOR_PATCH_STR "." __TO_STRING(PRODUCT_VERSION_BUILD)

#define PRODUCT_VERSION_STR        "@PRODUCT_VERSION@"
#define PRODUCT_NAME               "@PRODUCT_NAME@"
#define PRODUCT_COMMENTS           "@PRODUCT_COMMENTS@"
#define PRODUCT_COMPANY_NAME       "@PRODUCT_COMPANY_NAME@"
#define PRODUCT_COMPANY_COPYRIGHT  "@PRODUCT_COMPANY_COPYRIGHT@"
#define PRODUCT_FILE_DESCRIPTION   "@PRODUCT_FILE_DESCRIPTION@"
#define PRODUCT_INTERNAL_NAME      "@PRODUCT_NAME@"
#define PRODUCT_ORIGINAL_FILENAME  "@PRODUCT_ORIGINAL_FILENAME@"
#define PRODUCT_BUNDLE             "@PRODUCT_BUNDLE@"

#if @PRODUCT_PRIVATE_BUILD_FLAG@
#define PRODUCT_PRIVATE_BUILD      "@PRODUCT_PRIVATE_BUILD@"
#endif

#if @PRODUCT_SPECIAL_BUILD_FLAG@
#define PRODUCT_SPECIAL_BUILD      "@PRODUCT_SPECIAL_BUILD@"
#endif

#if @PRODUCT_PRERELEASE_FLAG@
#define PRODUCT_PRERELEASE         1
#endif

#if @PRODUCT_PATCHED_FLAG@
#define PRODUCT_PATCHED            1
#endif
