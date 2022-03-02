/**
 * @file
 * @brief      Implementation of Clang compiling helper class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#include "ClangCompileHelper.hpp"
#include "ClangHelper.hpp"
#include <iostream>

bool ClangCompileHelper::CheckMockCompilation( const std::string &inputHeader, const std::string &testedSource )
{
#ifdef DISABLE_COMPILATION_CHECK
    return true;
#else
    CXIndex index = clang_createIndex( 0, 0 );

    const char* clangOpts[] = { "-xc++", "-I" CPPUTEST_INCLUDE_DIR };

    std::string compiledCode =
            "#include <CppUTest/TestHarness.h>\n"
            "#include <CppUTestExt/MockSupport.h>\n";
#ifdef INTERPRET_C
    compiledCode += "extern \"C\" {\n";
#endif
    compiledCode += inputHeader + "\n";
#ifdef INTERPRET_C
    compiledCode += "}\n";
#endif
    compiledCode += testedSource;

    CXUnsavedFile unsavedFiles[] = { { "test_mock.cpp", compiledCode.c_str(), (unsigned long) compiledCode.length() }  };

    CXTranslationUnit tu = clang_parseTranslationUnit( index, "test_mock.cpp",
                                                       clangOpts, std::extent<decltype(clangOpts)>::value,
                                                       unsavedFiles, std::extent<decltype(unsavedFiles)>::value,
                                                       CXTranslationUnit_None );
    if( tu == nullptr )
    {
        clang_disposeIndex( index );
        throw std::runtime_error( "Error creating translation unit" );
    }

    unsigned int numDiags = clang_getNumDiagnostics(tu);
    if( numDiags > 0 )
    {
        std::cerr << std::endl;
        std::cerr << "---------------- Error compiling --------------" << std::endl;
        std::cerr << compiledCode << std::endl;
        std::cerr << "-----------------------------------------------" << std::endl;
        for( unsigned int i = 0; i < numDiags; i++ )
        {
            CXDiagnostic diag = clang_getDiagnostic( tu, i );

            std::cerr << clang_formatDiagnostic( diag, clang_defaultDiagnosticDisplayOptions() ) << std::endl;

            clang_disposeDiagnostic( diag );
        }
    }

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return ( numDiags == 0 );
#endif
}

bool ClangCompileHelper::CheckExpectationCompilation( const std::string &inputHeader, const std::string &testedHeader, const std::string &testedSource )
{
#ifdef DISABLE_COMPILATION_CHECK
    return true;
#else
    CXIndex index = clang_createIndex( 0, 0 );

    const char* clangOpts[] = { "-xc++", "-I" CPPUTEST_INCLUDE_DIR, "-I" CPPUMOCKGEN_INCLUDE_DIR };

    std::string compiledCode =
            "#include <CppUTest/TestHarness.h>\n"
            "#include <CppUTestExt/MockSupport.h>\n"
            "#include <CppUMockGen.hpp>\n";
#ifdef INTERPRET_C
    compiledCode += "extern \"C\" {\n";
#endif
    compiledCode += inputHeader + "\n";
#ifdef INTERPRET_C
    compiledCode += "}\n";
#endif
    compiledCode += testedHeader + "\n";
    compiledCode += testedSource;

    CXUnsavedFile unsavedFiles[] = { { "test_expect.cpp", compiledCode.c_str(), (unsigned long) compiledCode.length() }  };

    CXTranslationUnit tu = clang_parseTranslationUnit( index, "test_expect.cpp",
                                                       clangOpts, std::extent<decltype(clangOpts)>::value,
                                                       unsavedFiles, std::extent<decltype(unsavedFiles)>::value,
                                                       CXTranslationUnit_None );
    if( tu == nullptr )
    {
        clang_disposeIndex( index );
        throw std::runtime_error( "Error creating translation unit" );
    }

    unsigned int numDiags = clang_getNumDiagnostics(tu);
    if( numDiags > 0 )
    {
        std::cerr << std::endl;
        std::cerr << "---------------- Error compiling --------------" << std::endl;
        std::cerr << compiledCode << std::endl;
        std::cerr << "-----------------------------------------------" << std::endl;
        for( unsigned int i = 0; i < numDiags; i++ )
        {
            CXDiagnostic diag = clang_getDiagnostic( tu, i );

            std::cerr << clang_formatDiagnostic( diag, clang_defaultDiagnosticDisplayOptions() ) << std::endl;

            clang_disposeDiagnostic( diag );
        }
    }

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return ( numDiags == 0 );
#endif
}
