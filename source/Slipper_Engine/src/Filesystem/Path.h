#pragma once

#include "common_defines.h"

namespace Path
{
extern std::string get_engine_path();
extern std::string make_engine_relative_path_absolute(std::string_view RelativePath);
}  // namespace Path