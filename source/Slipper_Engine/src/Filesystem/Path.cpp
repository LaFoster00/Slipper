#include "Path.h"

namespace Path
{
	void ReplaceSubstring(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	std::string GetEnginePath()
	{
		char ModuleFileName[MAX_PATH];
#ifdef WINDOWS
		HMODULE hm = NULL;

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)&GetEnginePath, &hm) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
			// Return or however you want to handle an error.
		}
		if (GetModuleFileName(hm, ModuleFileName, MAX_PATH) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
			// Return or however you want to handle an error.
		}
#elif LINUX
		readlink("/proc/self/exe", ModuleFileName, MAX_PATH)
#endif
		std::string ModuleFileNameString(ModuleFileName);
		ReplaceSubstring(ModuleFileNameString, "\\", "/");
		ModuleFileNameString = ModuleFileNameString.erase(ModuleFileNameString.find_last_of('/') + 1);
		return std::string(ModuleFileNameString);
	}

	std::string MakeEngineRelativePathAbsolute(std::string_view RelativePath)
	{
		std::string finalPath(RelativePath);
		std::replace(finalPath.begin(), finalPath.end(), '\\', '/');
		if (RelativePath[1] == '/')
		{
			finalPath = finalPath.substr(2);
		}
		else
		{
			finalPath = finalPath.substr(1);
		}

		finalPath = std::string(Path::GetEnginePath()) + finalPath;
		return finalPath;
	}
}