#pragma once

#ifdef LINUX
#    include "Platform.h"
#    include <cstring>
#    include <string>
#endif

#ifdef LINUX
class Linux : public IPlatform
{
 public:
    static std::string GetBinaryPath();
};
#endif