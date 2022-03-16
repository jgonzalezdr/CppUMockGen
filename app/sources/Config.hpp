/**
 * @file
 * @brief      Header of Config class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2020 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CPPUMOCKGEN_CONFIG_HPP_
#define CPPUMOCKGEN_CONFIG_HPP_

#include <string>
#include <map>
#include <vector>
#include <utility>

enum class MockedType
{
    Bool,
    Int,
    UnsignedInt,
    Long,
    UnsignedLong,
    Double,
    String,
    Pointer,
    ConstPointer,
    Output,
    InputOfType,
    OutputOfType,
    MemoryBuffer,
    POD,
    Skip
};

/**
 * Class that manages the configuration to be used for function parsing, and mock generation.
 */
class Config
{
public:
    /**
     * Specification of the override to be applied to a function parameter / return type.
     */
    class OverrideSpec
    {
    public:
        /**
         * Constructs a OverrideSpec object.
         *
         * @param value [in] Override specification, consisting in a element identifier optionally followed
         *                   by the '=' character and an argument / return expression modifier
         * @param isReturn [in] Indicates that the option is for return type if @c true, or for parameter
         *                      type otherwise
         */
        OverrideSpec( const std::string &value, bool isReturn );

        /**
         * Returns the type identifier.
         */
        MockedType GetType() const noexcept;

        /**
         * Returns the exposed type name.
         * @return String with the exposed type name
         */
        const std::string& GetExposedTypeName() const noexcept;

        /**
         * Returns the expression modifier part before the character '$'.
         * @return String with the front expression modifier part
         */
        const std::string& GetExprModFront() const noexcept;

        /**
         * Returns the expression modifier part after the character '$'.
         * @return String with the back expression modifier part
         */
        const std::string& GetExprModBack() const noexcept;

        /**
         * Indicates if the size expression includes the character '$' as a parameter name placeholder.
         * 
         * @return @c true if the size expression has a parameter name placeholder, otherwise @c false.
         */
        const bool HasSizeExprPlaceholder() const noexcept;

        /**
         * Returns the part before the character '$' of the expression that calculates the memory buffer size.
         * @return String with the expression front part
         */
        const std::string& GetSizeExprFront() const noexcept;

        /**
         * Returns the part after the character '$' of the expression that calculates the memory buffer size.
         * @return String with the expression back part
         */
        const std::string& GetSizeExprBack() const noexcept;

        /**
         * Returns the expectation argument type name.
         * @return String with the expectation argument type name
         */
        const std::string& GetExpectationArgTypeName() const noexcept;

    private:
        MockedType m_type;
        std::string m_exposedTypeName;
        std::string m_exprModFront;
        std::string m_exprModBack;
        bool m_hasSizeExprPlaceholder;
        std::string m_sizeExprFront;
        std::string m_sizeExprBack;
        std::string m_expectationArgTypeName;
    };

    /**
     * Constructs a Config object.
     *
     * @param useUnderlyingTypedefType [in] Indicates whether to use the underlying type of typedefs or the typedef name
     *                                      to identify function parameters types
     * @param typeOverrideOptions [in] Override options for specific or generic parameter or return types (see OverrideMap)
     */
    Config( bool useUnderlyingTypedefType, const std::vector<std::string> &typeOverrideOptions );

    /**
     * Returns whether the underlying type of typedefs or the typedef name has to be used to identify function
     * parameters types
     */
    bool UseUnderlyingTypedefType() const noexcept;

    /**
     * Returns the override spec related to the parameter or return type represented
     * by @p key (if it exists).
     *
     * @param key [in] Identifier for a parameter or return type
     * @return A pointer to an override spec, or NULL if it does not exist
     */
    const OverrideSpec* GetTypeOverride( const std::string& key ) const noexcept;

private:
    /**
     * Correspondences between an element and its related override specification.
     */
    class OverrideMap
    {
    public:
        /**
         * Constructs an OverrideMap object.
         *
         * @param options [in] Override options, each consisting in a pair of element identifier
         *                     (key) / override specification (value) separated by a '=' character
         */
        OverrideMap( const std::vector<std::string> &options );

        /**
         * Returns the override specification related to an element represented by @p key
         * (if it exists).
         *
         * @param key [in] Identifier for an element
         * @return A pointer to an override specification, or NULL if it does not exist
         */
        const OverrideSpec* GetOverride( const std::string& key ) const noexcept;

    private:
        typedef std::map<std::string, OverrideSpec> OverrideMapType;
        OverrideMapType m_map;
    };

    bool m_useUnderlyingTypedefType;
    OverrideMap m_typeOverrideMap;
};

#endif // header guard
