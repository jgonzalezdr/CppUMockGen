#include "App.hpp"

#include <iostream>

int main( int argc, const char* argv[] )
{
    App app( std::cout, std::cerr );
    return app.Execute( argc, argv );
}
