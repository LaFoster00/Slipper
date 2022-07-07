#pragma once

#include <string>
#include <string_view>

namespace String
{
template<typename... FormatT>
requires std::is_convertible_v<FormatT..., std::string> std::string append(
    std::string Base, const FormatT &...Format)
{
    return Base + format(Format...);
}

template<typename... StringView>
requires std::is_convertible_v<StringView..., std::string_view>
int get_total_length_of_strings(const std::string_view &String, const StringView &...Strings)
{
    return String.length() + GetTotalLength(Strings...);
}

extern void replace_substring(std::string &Str, const std::string &From, const std::string &To);
}  // namespace String