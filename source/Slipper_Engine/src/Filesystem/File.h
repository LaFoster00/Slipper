#pragma once

#include <vector>
#include <string>

namespace File
{
    extern std::vector<char> ReadBinaryFile(const char *filepath);
    extern std::string GetFileNameFromPath(const char *filepath);
}
