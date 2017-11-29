#ifndef CLANG_TEST_HELPER_HPP_
#define CLANG_TEST_HELPER_HPP_

#include <clang-c/Index.h>
#include <string>
#include <functional>

class ClangTestHelper
{
public:
    ClangTestHelper( const std::string &code );
    ~ClangTestHelper();

    void ParseFunctions( const std::function<void( CXCursor )> &callback );

private:
    std::string m_code;
};

#endif // header guard
