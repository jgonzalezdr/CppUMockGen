/**
 * @file
 * @brief      Header of ConsoleColorizer class
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2017-2018 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CONSOLECOLORIZED_HPP_
#define CONSOLECOLORIZED_HPP_

#ifdef WIN32
#include "windows.h"
#endif

/**
 * Helper class to colorize console output.
 */
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

    /**
     * Constructor.
     *
     * @param[in] consoleType Console to colorize
     */
    ConsoleColorizer( ConsoleType consoleType ) noexcept;

    /**
     * Destructor.
     */
    ~ConsoleColorizer() noexcept;

    /**
     * Sets (or resets) the console color.
     *
     * @param[in] color Color to be set, or RESET to reset to default value
     */
    void SetColor( Color color ) noexcept;

private:
#ifdef WIN32
    HANDLE m_handle;
    WORD m_origConsoleAttrs;
#endif
};

extern ConsoleColorizer cerrColorizer;

#endif // header guard
