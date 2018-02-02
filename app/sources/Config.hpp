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
         * @param value [in] Override specification, consisting in a type identifier optionally followed
         *                   by the '@' character and an argument expression modifier.
         */
        OverrideSpec( const std::string &value, const std::string &option );

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
     * @param overrideOptions [in] Override options (see OverrideMap)
     */
    Config( bool useUnderlyingTypedefType, const std::vector<std::string> &overrideOptions );

    /**
     * Returns whether the underlying type of typedefs or the typedef name has to be used to identify function or method
     * parameters types
     */
    bool UseUnderlyingTypedefType() const;

    /**
     * Returns the override spec related to the function or method parameter represented by @p key (if it exists).
     *
     * @param key [in] Identifier for a function or method parameter
     * @return A pointer to an override spec, or NULL if it does not exist
     */
    const OverrideSpec* GetOverride( const std::string& key ) const;

private:
    /**
     * Correspondences between function or method parameters and its related override specification.
     */
    class OverrideMap
    {
    public:
        /**
         * Constructs an OverrideMap object.
         *
         * @param options [in] Override options, each consisting in a pair of function or method parameter identifier
         *                     (key) / override specification (value) separated by a '=' character
         */
        OverrideMap( const std::vector<std::string> &options );

        /**
         * Returns the override specification related to the function or method parameter represented by @p key
         * (if it exists).
         *
         * @param key [in] Identifier for a function or method parameter
         * @return A pointer to an override specification, or NULL if it does not exist
         */
        const OverrideSpec* GetOverride( const std::string& key ) const;

    private:
        typedef std::map<std::string, OverrideSpec> OverrideMapType;
        OverrideMapType m_map;
    };

    bool m_useUnderlyingTypedefType;
    OverrideMap m_overrideMap;
};

#endif // header guard
