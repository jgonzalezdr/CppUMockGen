#ifndef CLANG_COMPILE_HELPER_HPP_
#define CLANG_COMPILE_HELPER_HPP_

#include <clang-c/Index.h>
#include <string>
#include <functional>

class ClangCompileHelper
{
public:
    static bool CheckCompilation( const std::string &testedHeader, const std::string &testedSource );

private:
    ClangCompileHelper() {};
};

#endif // header guard
