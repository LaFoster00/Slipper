#pragma once
#include "Texture.h"

namespace Slipper
{
class DepthBuffer : public Texture
{
public:
    //MSAA Settings set in Engine::Settings::MSAA_SAMPLES
    DepthBuffer(VkExtent2D Extent,
                VkFormat Format);
};
}