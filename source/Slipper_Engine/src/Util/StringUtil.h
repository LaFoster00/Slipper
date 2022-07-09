#pragma once

#include <cstdarg>
#include <string>
#include <string_view>

namespace String
{
inline std::string append(std::string String)
{
    return String;
}

template<typename StringConvertible>
concept Convertible = std::is_convertible_v<StringConvertible, std::string>;

template<Convertible... StringT> inline std::string append(std::string Base, StringT... A)
{
    return Base + append(A...);
}

inline std::string format(const char *Base, ...)
{
    std::string final_string;
    size_t final_string_size = strlen(Base);

    va_list args;
    va_start(args, Base);
    final_string_size += va_arg(args, std::string).size();
    va_end(args);
    final_string.resize(final_string_size);

    va_start(args, Base);
    vsprintf_s(final_string.data(), final_string.size(), Base, args);
    va_end(args);

    return final_string;
}

template<typename... StringView>
requires std::is_convertible_v<StringView..., std::string_view>
int get_total_length_of_strings(const std::string_view &String, const StringView &...Strings)
{
    return String.length() + GetTotalLength(Strings...);
}

extern void replace_substring(std::string &Str, const std::string &From, const std::string &To);

extern std::string to_lower(std::string_view String);
}  // namespace String