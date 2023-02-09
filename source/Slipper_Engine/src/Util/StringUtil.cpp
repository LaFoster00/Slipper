#include "StringUtil.h"

#include <algorithm>
#include <string>
#include <string_view>

namespace Slipper
{
namespace String
{
void replace_substring(std::string &Str, const std::string &From, const std::string &To)
{
    if (!From.empty()) {
        size_t start_pos = 0;
        while ((start_pos = Str.find(From, start_pos)) != std::string::npos) {
            Str.replace(start_pos, From.length(), To);
            start_pos +=
                To.length();  // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
}

std::string to_lower(std::string_view String)
{
    std::string output(String);
    std::ranges::transform(
        String, output.begin(), [](unsigned char c) { return std::tolower(c); });
    return output;
}
}  // namespace String
}  // namespace Slipper