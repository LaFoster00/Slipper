#pragma once

#include "common_defines.h"
#include <algorithm>

#ifdef WINDOWS
#define NOMINMAX
#include "windows.h"
#endif

namespace Path
{
	extern void ReplaceSubstring(std::string& str, const std::string& from, const std::string& to);
	extern std::string GetEnginePath();
	extern std::string MakeEngineRelativePathAbsolute(std::string_view RelativePath);
}