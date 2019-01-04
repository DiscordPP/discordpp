#ifndef DISCORDPP_COLORS_HH
#define DISCORDPP_COLORS_HH

#include <string>
#include <sstream>

namespace discordpp
{

    enum Color
    {
        Reset = 0,
        Bold,
        Faint,
        Italic,
        Underline,
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    void color(std::string str, Color color)
    {
        std::string clrstr = std::to_string(color);
        std::ostringstream clr;

        clr << "\033["
            << clrstr
            << "m"
            << str
            << "\033["
            << Color.Reset
            << "m";
        return clr;
    }
}

#endif//DISCORDPP_COLORS_HH