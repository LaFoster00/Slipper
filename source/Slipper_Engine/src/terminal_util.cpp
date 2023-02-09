#include "terminal_util.h"

namespace Slipper
{
namespace TerminalUtil
{
const char *ansiColorEscape[]{
    "\x1B[30m",
    "\x1B[31m",
    "\x1B[32m",
    "\x1B[33m",
    "\x1B[34m",
    "\x1B[35m",
    "\x1B[36m",
    "\x1B[37m",
};

const char *ColorText(TextColor color, const char *text)
{
    static std::string str;
    str = "";
    str += ansiColorEscape[static_cast<uint32_t>(color)];
    str += text;
    str += "\033[0m";
    return str.c_str();
}
}  // namespace TerminalUtil
}  // namespace Slipper