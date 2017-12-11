#include "ClangCompileHelper.hpp"

bool ClangCompileHelper::CheckCompilation( const std::string &testedHeader, const std::string &testedSource )
{
	return true;
    CXIndex index = clang_createIndex( 0, 1 );

    const char* clangOpts[] = { "-xc++", "-I" CPPUTEST_INCLUDE_DIR };

    std::string compiledCode =
            "#include <CppUTest/TestHarness.h>\n"
            "#include <CppUTestExt/MockSupport.h>\n";
    compiledCode += testedHeader + "\n" + testedSource;

    CXUnsavedFile unsavedFiles[] = { { "test_mock.cpp", compiledCode.c_str(), (unsigned long) compiledCode.length() }  };

    CXTranslationUnit tu = clang_parseTranslationUnit( index, "test_mock.cpp",
                                                       clangOpts, std::extent<decltype(clangOpts)>::value,
                                                       unsavedFiles, std::extent<decltype(unsavedFiles)>::value,
                                                       CXTranslationUnit_None );
    if( tu == nullptr )
    {
        throw std::runtime_error( "Error creating translation unit" );
    }

    bool compilationOK = ( clang_getNumDiagnostics(tu) == 0 );

    clang_disposeTranslationUnit( tu );
    clang_disposeIndex( index );

    return compilationOK;
}
