#pragma once

#include <vector>
#include <string>

namespace File
{
    extern std::vector<char> ReadBinaryFile(std::string_view filepath);
    extern std::string GetFileNameFromPath(std::string_view filepath);
}
