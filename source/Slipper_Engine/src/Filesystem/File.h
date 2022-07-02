#pragma once

#include <string>
#include <vector>

namespace File
{
extern std::vector<char> read_binary_file(std::string_view Filepath);
extern std::string get_file_name_from_path(std::string_view Filepath);
}  // namespace File
