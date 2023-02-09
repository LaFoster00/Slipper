#pragma once

#ifdef LINUX
#    include "Platform.h"
#    include <cstring>
#    include <string>
#endif

#ifdef LINUX
namespace Slipper
{
class Linux : public IPlatform
{
 public:
    static std::string GetBinaryPath();
};
}
#endif