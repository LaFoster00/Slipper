#pragma once

#include "common_defines.h"
#include <algorithm>

#ifdef WINDOWS
#    define NOMINMAX
#    include "windows.h"
#endif

namespace Path
{
extern void replace_substring(std::string &Str, const std::string &From, const std::string &To);
extern std::string get_engine_path();
extern std::string make_engine_relative_path_absolute(std::string_view RelativePath);
}  // namespace Path