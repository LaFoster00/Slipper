// NOT WORKING
#include <string>

namespace TerminalUtil
{
    enum class TextColor
    {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37
    };

    extern const char *ansiColorEscape[];

    extern const char *ColorText(TextColor color, const char *text);

} // namespace TerminalUtil