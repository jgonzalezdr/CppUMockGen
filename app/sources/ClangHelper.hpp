#ifndef CLANGHELPER_HPP_
#define CLANGHELPER_HPP_

#include <iostream>
#include <string>
#include <clang-c/Index.h>

/**
 * Writes the CXString @p cxstr to @p stream.
 *
 * @param stream [out] An output stream
 * @param cxstr [in] A CXString
 * @return @p stream
 */
std::ostream& operator<<( std::ostream& stream, const CXString& cxstr );

/**
 * Converts the CXString @p cxstr to a string.
 *
 * @param cxstr [in] A CXString
 * @return Result of conversion
 */
std::string toString( const CXString& cxstr );

/**
 * Operator that concatenates the CXString @p cxstr to @p string.
 *
 * @param string [in/out] A string
 * @param cxstr [in] A CXString
 * @return @p string
 */
std::string& operator+=( std::string &string, const CXString& cxstr );

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param string [in] A string
 * @param cxstr [in] A CXString
 * @return Result of the concatenation
 */
std::string operator+( const std::string &string, const CXString& cxstr );

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param string [in] A string
 * @param cxstr [in] A CXString
 * @return Result of the concatenation
 */
std::string operator+( const char *string, const CXString& cxstr );

/**
 * Returns the qualified name of a function, method or attribute (i.e., prefixed with namespaces and class names).
 *
 * @param cursor [in] A cursor
 * @return A string with the fill qualified name of the cursor
 */
std::string getQualifiedName( const CXCursor &cursor );

/**
 * Returns the spelling of a type removing modifiers (const, volatile, pointer, etc.).
 *
 * @param type [in] A type
 * @return A string with the spelling of the bare type
 */
std::string getBareTypeSpelling( const CXType &type );

#endif // header guard
