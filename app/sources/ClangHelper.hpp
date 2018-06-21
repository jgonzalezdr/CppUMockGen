#ifndef CPPUMOCKGEN_CLANGHELPER_HPP_
#define CPPUMOCKGEN_CLANGHELPER_HPP_

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
std::ostream& operator<<( std::ostream& stream, const CXString& cxstr ) noexcept;

/**
 * Converts the CXString @p cxstr to a string.
 *
 * @param cxstr [in] A CXString
 * @return Result of conversion
 */
std::string toString( const CXString& cxstr ) noexcept;

/**
 * Operator that concatenates the CXString @p cxstr to @p string.
 *
 * @param string [in/out] A string
 * @param cxstr [in] A CXString
 * @return @p string
 */
std::string& operator+=( std::string &string, const CXString& cxstr ) noexcept;

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param string [in] A string
 * @param cxstr [in] A CXString
 * @return Result of the concatenation
 */
std::string operator+( const std::string &string, const CXString& cxstr ) noexcept;

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param string [in] A string
 * @param cxstr [in] A CXString
 * @return Result of the concatenation
 */
std::string operator+( const char *string, const CXString& cxstr ) noexcept;

/**
 * Returns the qualified name of a function or variable (i.e., prefixed with namespaces and class names).
 *
 * @param cursor [in] A cursor
 * @return A string with the full qualified name of the cursor
 */
std::string getQualifiedName( const CXCursor &cursor ) noexcept;

/**
 * Returns the spelling of a type removing modifiers (const, volatile, pointer, etc.).
 *
 * @param type [in] A type
 * @return A string with the spelling of the bare type
 */
std::string getBareTypeSpelling( const CXType &type ) noexcept;

/**
 * Returns the name of the class to which a method belongs.
 *
 * @param cursor [in] A cursor to a method
 * @return A string with name of the class to which belongs the method of the cursor
 */
std::string getMethodClassName( const CXCursor &cursor ) noexcept;

/**
 * Checks if a method and its containing classes have public scope.
 *
 * @param cursor [in] A cursor to a method
 * @return @c true if the method and its containing classes are public, or @c false otherwise
 */
bool isMethodPublic( const CXCursor &cursor ) noexcept;

#endif // header guard
