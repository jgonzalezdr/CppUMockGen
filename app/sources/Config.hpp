#ifndef CONFIG_HPP_
#define CONFIG_HPP_

class Config
{
public:
    Config( bool useUnderlyingTypedefType )
    : m_useUnderlyingTypedefType( useUnderlyingTypedefType )
    {};

    bool UseUnderlyingTypedefType() const
    {
        return m_useUnderlyingTypedefType;
    }

private:
    bool m_useUnderlyingTypedefType;
};

#endif // header guard
