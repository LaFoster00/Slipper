#pragma once

#include <string>
#include <string_view>

namespace String
{
template<typename... Format>
requires std::is_convertible_v<Format..., std::string_view> std::string format(
    const std::string_view &Base, const Format &...format)
{
    size_t finalStringSize = get_total_length_of_strings(format...);

    const auto final_message = static_cast<char *>(malloc(Base.length() + finalStringSize));

    sprintf(final_message, Base.data(), format.data()...);
    return final_message;
}

template<typename... StringView>
requires std::is_convertible_v<StringView..., std::string_view>
int get_total_length_of_strings(const std::string_view &String, const StringView &...Strings)
{
    return String.length() + GetTotalLength(Strings...);
}

extern void replace_substring(std::string &Str, const std::string &From, const std::string &To);
}  // namespace String