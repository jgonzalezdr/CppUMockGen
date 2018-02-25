#ifndef CPPUMOCKGEN_APP_HPP_
#define CPPUMOCKGEN_APP_HPP_

#include <ostream>

class App
{
public:
    App( std::ostream &cout, std::ostream &cerr );

    int Execute( int argc, char* argv[] );

private:
    void PrintError( const char *msg );

    std::ostream &m_cout;
    std::ostream &m_cerr;
};

#endif // header guard
