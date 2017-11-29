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

/**
 * Returns the qualified name of a function, method or attribute (i.e., prefixed with namespaces and class names).
 * @param cursor A cursor
 * @return A string with the fill qualified name of the cursor
 */
std::string getQualifiedName( const CXCursor &cursor );

/**
 * Returns the spelling of a type removing modifiers (const, volatile, pointer, etc.).
 * @param type A type
 * @return A string with the spelling of the bare type
 */
std::string getBareTypeSpelling( const CXType &type );

#endif // header guard
