/*
 * This file has been auto-generated by CppUTestMock v0.1.0.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 */

#include "ConsoleColorizer.hpp"

#include <CppUTestExt/MockSupport.h>

ConsoleColorizer cerrColorizer( ConsoleColorizer::ConsoleType::STD_ERROR );

ConsoleColorizer::ConsoleColorizer( ConsoleType consoleType )
: m_handle( NULL )
{
}

ConsoleColorizer::~ConsoleColorizer()
{
}

void ConsoleColorizer::SetColor(ConsoleColorizer::Color color)
{
    mock().actualCall("ConsoleColorizer::SetColor").withIntParameter("color", static_cast<int>(color));
}

