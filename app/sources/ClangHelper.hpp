/**
 * @file
 * @brief      Header of Clang helper functions
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_CLANGHELPER_HPP_
#define CPPUMOCKGEN_CLANGHELPER_HPP_

#include <iostream>
#include <string>
#include <clang-c/Index.h>

/**
 * Writes the CXString @p cxstr to @p stream.
 *
 * @param[out] stream An output stream
 * @param[in] cxstr A CXString
 * @return @p stream
 */
std::ostream& operator<<( std::ostream& stream, const CXString& cxstr ) noexcept;

/**
 * Converts the CXString @p cxstr to a string.
 *
 * @param[in] cxstr A CXString
 * @return Result of conversion
 */
std::string toString( const CXString& cxstr ) noexcept;

/**
 * Operator that concatenates the CXString @p cxstr to @p string.
 *
 * @param[in,out] string A string
 * @param[in] cxstr A CXString
 * @return @p string
 */
std::string& operator+=( std::string &string, const CXString& cxstr ) noexcept;

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param[in] string A string
 * @param[in] cxstr A CXString
 * @return Result of the concatenation
 */
std::string operator+( const std::string &string, const CXString& cxstr ) noexcept;

/**
 * Operator that returns the concatenation of @p string and CXString @p cxstr.
 *
 * @param[in] string A string
 * @param[in] cxstr A CXString
 * @return Result of the concatenation
 */
std::string operator+( const char *string, const CXString& cxstr ) noexcept;

/**
 * Returns the qualified name of a function or variable (i.e., prefixed with namespaces and class names).
 *
 * @param[in] cursor A cursor
 * @return A string with the full qualified name of the cursor
 */
std::string getQualifiedName( const CXCursor &cursor ) noexcept;

/**
 * Returns the spelling of a type removing modifiers (const, volatile, pointer, etc.).
 *
 * @param[in] type A type
 * @return A string with the spelling of the bare type
 */
std::string getBareTypeSpelling( const CXType &type ) noexcept;

/**
 * Returns the name of the class to which a member belongs.
 *
 * @param[in] cursor A cursor to a member
 * @return A string with name of the class to which the member of the cursor belongs
 */
std::string getMemberClassName( const CXCursor &cursor ) noexcept;

/**
 * Checks if a member and its containing classes have public scope.
 *
 * @param[in] cursor A cursor to a member
 * @return @c true if the member and its containing classes are public, or @c false otherwise
 */
bool isCursorPublic( const CXCursor &cursor ) noexcept;

/**
 * Checks if a member has non-private scope and its containing classes have public scope.
 *
 * @param[in] cursor A cursor to a member
 * @return @c true if the member is non-private and its containing classes are public, or @c false otherwise
 */
bool isCursorNonPrivate( const CXCursor &cursor ) noexcept;

/**
 * Checks if a member's containing classes have public scope.
 *
 * @param[in] cursor A cursor to a member
 * @return @c true if the member's containing classes are public, or @c false otherwise
 */
bool isCursorInPublicClass( const CXCursor &cursor ) noexcept;

#endif // header guard
