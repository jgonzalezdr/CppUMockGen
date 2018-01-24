#ifndef CONSOLECOLORIZED_HPP_
#define CONSOLECOLORIZED_HPP_

#ifdef WIN32
#include "windows.h"
#endif

class ConsoleColorizer
{
public:
    enum class ConsoleType { STD_OUTPUT, STD_ERROR };
    enum class Color
    {
        BLACK,
        DARK_BLUE,
        DARK_GREEN,
        DARK_CYAN,
        DARK_RED,
        DARK_MAGENTA,
        BROWN,
        LIGHT_GRAY,
        DARK_GRAY,
        LIGHT_BLUE,
        LIGHT_GREEN,
        LIGHT_CYAN,
        LIGHT_RED,
        LIGHT_MAGENTA,
        YELLOW,
        WHITE,
        RESET
    };

    ConsoleColorizer( ConsoleType consoleType );
    ~ConsoleColorizer();

    void SetColor( Color color );

private:
#ifdef WIN32
    HANDLE m_handle;
    WORD m_origConsoleAttrs;
#endif
};

extern ConsoleColorizer cerrColorizer;

#endif // header guard
