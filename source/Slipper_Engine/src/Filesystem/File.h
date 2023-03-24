#pragma once

namespace Slipper
{
namespace File
{
extern std::vector<char> read_binary_file(std::string_view Filepath);
extern std::string get_file_name_from_path(std::string_view Filepath);
extern std::string remove_file_type_from_name(std::string_view FileName);
	extern std::string get_file_ending_from_path(std::string_view Filepath);
extern std::string get_shader_type_from_spirv_path(std::string_view Filepath);
}  // namespace File
}