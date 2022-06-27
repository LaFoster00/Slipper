#pragma once

#include <algorithm>

#define NOMINMAX
#include "windows.h"

namespace Path
{
	std::string GetEnginePath()
	{
		char ModuleFileName[MAX_PATH];
		GetModuleFileName(nullptr, ModuleFileName, MAX_PATH);
		return std::string(ModuleFileName);
	}

	std::string MakeEngineRelativePathAbsolute(std::string_view RelativePath)
	{
		std::string finalPath(RelativePath);
		std::replace(finalPath.begin(), finalPath.end(), '\\', '/');
		if (RelativePath[1] == '/')
		{
			finalPath = finalPath.substr(1);
		}
		else
		{
			finalPath = finalPath.substr(2);
		}
			
		finalPath = std::string(Path::GetEnginePath()) + finalPath;
		return finalPath;
	}
}