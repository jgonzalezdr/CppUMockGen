#ifndef CLANG_PARSE_HELPER_HPP_
#define CLANG_PARSE_HELPER_HPP_

#include <clang-c/Index.h>
#include <string>
#include <functional>

class ClangParseHelper
{
public:
    static void ParseHeader( const std::string &code, const std::function<void( CXCursor )> &callback );

private:
    ClangParseHelper() {};
};

#endif // header guard
