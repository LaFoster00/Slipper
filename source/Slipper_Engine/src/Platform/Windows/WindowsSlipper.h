#pragma once

#include "Platform.h"
#include <string>

#ifdef WINDOWS
#    define NOMINMAX
#    include "windows.h"
#endif

#ifdef WINDOWS
class Windows : public IPlatform
{
 public:
    static std::string GetBinaryPath()
    {
        static std::string module_file_name_string;
        if (module_file_name_string.empty()) {
            char module_file_name[MAX_PATH];
            HMODULE hm = nullptr;

            if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                  reinterpret_cast<LPCSTR>(&get_engine_path),
                                  &hm) == 0) {
                const int ret = GetLastError();
                fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
                // Return or however you want to handle an error.
            }
            if (GetModuleFileName(hm, module_file_name, MAX_PATH) == 0) {
                const int ret = GetLastError();
                fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
                // Return or however you want to handle an error.
            }

            module_file_name_string = module_file_name;
            replace_substring(module_file_name_string, "\\", "/");
            module_file_name_string = module_file_name_string.erase(
                module_file_name_string.find_last_of('/') + 1);
        }
        return module_file_name_string;
    }
};
#endif