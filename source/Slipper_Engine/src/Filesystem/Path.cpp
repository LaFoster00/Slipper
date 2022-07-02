#include "Path.h"

namespace Path
{
void replace_substring(std::string &Str, const std::string &From, const std::string &To)
{
    if (!From.empty())
    {
	    size_t start_pos = 0;
	    while ((start_pos = Str.find(From, start_pos)) != std::string::npos)
	    {
		    Str.replace(start_pos, From.length(), To);
		    start_pos += To.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	    }
    }
}

std::string get_engine_path()
{
    char ModuleFileName[MAX_PATH];
#ifdef WINDOWS
    HMODULE hm = NULL;

    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR)&get_engine_path,
                          &hm) == 0) {
	    const int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    if (GetModuleFileName(hm, ModuleFileName, MAX_PATH) == 0) {
	    const int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
#elif LINUX
    readlink("/proc/self/exe", ModuleFileName, MAX_PATH)
#endif
    std::string module_file_name_string(ModuleFileName);
    replace_substring(module_file_name_string, "\\", "/");
    module_file_name_string = module_file_name_string.erase(module_file_name_string.find_last_of('/') + 1);
    return std::string(module_file_name_string);
}

std::string make_engine_relative_path_absolute(std::string_view RelativePath)
{
    std::string final_path(RelativePath);
    std::replace(final_path.begin(), final_path.end(), '\\', '/');
    if (RelativePath[1] == '/') {
        final_path = final_path.substr(2);
    }
    else {
        final_path = final_path.substr(1);
    }

    final_path = std::string(Path::get_engine_path()) + final_path;
    return final_path;
}
}  // namespace Path