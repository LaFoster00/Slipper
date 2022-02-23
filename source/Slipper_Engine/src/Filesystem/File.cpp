#include "File.h"

#include "common_defines.h"

#include <fstream>

namespace File
{
    std::vector<char> ReadBinaryFile(const char *filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        ASSERT(!file.is_open(), "Failed to open file!")

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    /* Needs further refinement! */
    std::string GetFileNameFromPath(const char *filepath)
    {
        std::string fp(filepath);
        fp = fp.substr(fp.find_last_of('/') + 1);
        return fp.substr(0, fp.find_last_of('.'));
    }
}