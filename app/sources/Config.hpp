#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <map>
#include <vector>
#include <utility>

/**
 * Configuration to be used for mock generation.
 */
class Config
{
public:
    /**
     * Specification of the override to be applied to a function or method parameter.
     */
    class OverrideSpec
    {
    public:
        /**
         * Constructs a OverrideSpec object.
         *
         * @param value [in] Override specification, consisting in a element identifier optionally followed
         *                   by the '=' character and an argument / return expression modifier
         * @param option [in] Full override option text
         * @param isReturn [in] Indicates that the option is for return type if @c true, or for parameter
         *                      type otherwise
         */
        OverrideSpec( const std::string &value, const std::string &option, bool isReturn );

        /**
         * Returns the type identifier.
         */
        const std::string& GetType() const;

        /**
         * Returns the argument expression modifier part before the character '$'.
         * @return String with the front argument expression modifier part
         */
        const std::string& GetArgExprModFront() const;

        /**
         * Returns the argument expression modifier part after the character '$'.
         * @return String with the back argument expression modifier part
         */
        const std::string& GetArgExprModBack() const;

    private:
        std::string m_type;
        std::string m_argExprModFront;
        std::string m_argExprModBack;
    };

    /**
     * Constructs a Config object.
     *
     * @param useUnderlyingTypedefType [in] Indicates whether to use the underlying type of typedefs or the typedef name
     *                                      to identify function or method parameters types
     * @param paramOverrideOptions [in] Override options for specific function or method parameter or return types (see OverrideMap)
     * @param typeOverrideOptions [in] Override options for generic parameter or return types (see OverrideMap)
     */
    Config( bool useUnderlyingTypedefType, const std::vector<std::string> &paramOverrideOptions,
            const std::vector<std::string> &typeOverrideOptions );

    /**
     * Returns whether the underlying type of typedefs or the typedef name has to be used to identify function or method
     * parameters types
     */
    bool UseUnderlyingTypedefType() const;

    /**
     * Returns the override spec related to the specific function or method parameter or return
     * type represented by @p key (if it exists).
     *
     * @param key [in] Identifier for a specific function or method parameter or return type
     * @return A pointer to an override spec, or NULL if it does not exist
     */
    const OverrideSpec* GetParameterOverride( const std::string& key ) const;

    /**
     * Returns the override spec related to the generic parameter or return type represented
     * by @p key (if it exists).
     *
     * @param key [in] Identifier for a parameter or return type
     * @return A pointer to an override spec, or NULL if it does not exist
     */
    const OverrideSpec* GetTypeOverride( const std::string& key ) const;

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
         * @param typeOverride [in] Must be @p true for type overrides, or @false otherwise
         */
        OverrideMap( const std::vector<std::string> &options, bool typeOverride );

        /**
         * Returns the override specification related to an element represented by @p key
         * (if it exists).
         *
         * @param key [in] Identifier for an element
         * @return A pointer to an override specification, or NULL if it does not exist
         */
        const OverrideSpec* GetOverride( const std::string& key ) const;

    private:
        typedef std::map<std::string, OverrideSpec> OverrideMapType;
        OverrideMapType m_map;
    };

    bool m_useUnderlyingTypedefType;
    OverrideMap m_paramOverrideMap;
    OverrideMap m_typeOverrideMap;
};

#endif // header guard
