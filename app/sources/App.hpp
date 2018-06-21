#ifndef CPPUMOCKGEN_APP_HPP_
#define CPPUMOCKGEN_APP_HPP_

#include <ostream>

class App
{
public:
    App( std::ostream &cout, std::ostream &cerr ) noexcept;

    int Execute( int argc, const char* argv[] ) noexcept;

private:
    void PrintError( const char *msg ) noexcept;

    std::ostream &m_cout;
    std::ostream &m_cerr;
};

#endif // header guard
