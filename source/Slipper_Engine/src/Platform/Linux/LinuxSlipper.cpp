#include "LinuxSlipper.h"

#ifdef LINUX
#    include "Platform.h"
#    include "Util/StringUtil.h"
#    include "common_defines.h"

#    ifndef _GNU_SOURCE
#        define _GNU_SOURCE
#    endif
#    include <dlfcn.h>
#    include <linux/limits.h>
#    include <unistd.h>
#endif

#ifdef LINUX
namespace Slipper
{
std::string Linux::GetBinaryPath()
{
    static std::string module_file_name_string;
    if (module_file_name_string.empty()) {
        char module_file_name[MAX_PATH];
        readlink("/proc/self/exe", module_file_name, MAX_PATH);

        if (strlen(module_file_name) == 0) {
            Dl_info dl_info;
            dladdr((void *)GetBinaryPath, &dl_info);
            strcpy(module_file_name, reinterpret_cast<const char *>(dl_info.dli_fbase));
        }

        module_file_name_string = module_file_name;
        String::replace_substring(module_file_name_string, "\\", "/");
        module_file_name_string = module_file_name_string.erase(
            module_file_name_string.find_last_of('/') + 1);
    }
    return std::string(module_file_name_string);
}
}
#endif