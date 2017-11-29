#ifndef CLANGHELPER_HPP_
#define CLANGHELPER_HPP_

#include <iostream>
#include <string>
#include <clang-c/Index.h>

std::ostream& operator<<( std::ostream& stream, const CXString& cxstr );

std::string toString( const CXString& cxstr );

std::string& operator+=( std::string &string, const CXString& cxstr );
std::string operator+( const std::string &string, const CXString& cxstr );
std::string operator+( const char *string, const CXString& cxstr );

std::string getQualifiedName( const CXCursor &cursor );
std::string getBareTypeSpelling( const CXType &type );

#endif // header guard