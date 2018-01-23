#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <map>
#include <vector>
#include <utility>

class Config
{
public:
    class OverrideSpec
    {
    public:
        OverrideSpec( const std::string &value, const std::string &option );

        const std::string& GetType() const;

        void UpdateArgExpr( std::string &argExpr ) const;

    private:
        std::string m_type;
        std::string m_argExp;
    };

    class OverrideMap
    {
    public:
        OverrideMap( const std::vector<std::string> &options );

        const OverrideSpec* GetOverride( const std::string& key ) const;

    private:
        typedef std::map<std::string, OverrideSpec> OverrideMapType;
        OverrideMapType m_map;
    };

    Config( bool useUnderlyingTypedefType, const std::vector<std::string> &options );

    bool UseUnderlyingTypedefType() const;

    const OverrideSpec* GetOverride( const std::string& key ) const;

private:
    bool m_useUnderlyingTypedefType;
    OverrideMap m_overrideMap;
};

#endif // header guard
