#include "Path.h"

#ifdef WINDOWS
#    include "Windows/WindowsSlipper.h"
#endif
#ifdef LINUX
#    include "Linux/LinuxSlipper.h"
#endif

namespace Slipper
{
namespace Path
{
std::string get_engine_path()
{
#ifdef WINDOWS
    return Windows::GetBinaryPath();
#else
    return Linux::GetBinaryPath();
#endif
}

std::string make_engine_relative_path_absolute(const std::string_view RelativePath)
{
    std::string final_path(RelativePath);
    std::ranges::replace(final_path, '\\', '/');

    if (RelativePath.find("./", 0) == std::string::npos) {
        constexpr char base_message[] =
            "Relative Path '%s' does not start with './' or '.\\'. It is therefore not valid.";
        const auto final_message = static_cast<char *>(
            malloc(sizeof(base_message) + RelativePath.length()));

        sprintf(final_message, base_message, RelativePath.data());
        throw std::runtime_error(final_message);
    }

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
}  // namespace Slipper