#include "File.h"

#include "Path.h"

namespace Slipper
{
namespace File
{
// Mark relative path with "./"
std::vector<char> read_binary_file(const std::string_view Filepath)
{
    std::string final_path(Filepath);
    if (*Filepath.begin() == '.') {
        final_path = Path::make_engine_relative_path_absolute(Filepath);
    }
    std::ifstream file(final_path.data(), std::ios::ate | std::ios::binary);

    ASSERT(!file.is_open(), "Failed to open file!")

    const size_t file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();

    return buffer;
}

/* Needs further refinement! */
std::string get_file_name_from_path(const std::string_view Filepath)
{
    std::string fp(Filepath);
    if (*Filepath.begin() == '.') {
        std::ranges::replace(fp, '\\', '/');
    }
    fp = fp.substr(fp.find_last_of('/') + 1);
    return fp.substr(0, fp.find_last_of('.'));
}
}  // namespace File
}  // namespace Slipper