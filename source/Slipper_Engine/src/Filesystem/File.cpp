#include "File.h"

#include "common_defines.h"
#include "Path.h"

#include <fstream>
#include <algorithm>

namespace File
{
    // Mark relative path with "./"
    std::vector<char> ReadBinaryFile(std::string_view filepath)
    {
        std::string finalPath(filepath);
        if (*filepath.begin() == '.')
        {
            finalPath = Path::MakeEngineRelativePathAbsolute(filepath);
        }
        std::ifstream file(finalPath.data(), std::ios::ate | std::ios::binary);

        ASSERT(!file.is_open(), "Failed to open file!")

        const size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    /* Needs further refinement! */
    std::string GetFileNameFromPath(std::string_view filepath)
    {
        std::string fp(filepath);
        if (*filepath.begin() == '.')
        {
            std::replace(fp.begin(), fp.end(), '\\', '/');
        }
        fp = fp.substr(fp.find_last_of('/') + 1);
        return fp.substr(0, fp.find_last_of('.'));
    }
}