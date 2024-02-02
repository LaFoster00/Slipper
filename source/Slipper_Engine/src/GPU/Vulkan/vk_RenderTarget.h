#pragma once

#include "vk_Texture.h"

namespace Slipper::GPU::Vulkan
{
class RenderTarget : public Texture
{
public:
    // MSAA Settings set in Engine::Settings::MSAA_SAMPLES
    RenderTarget(
            VkExtent2D Extent,
            vk::Format Format);
};
}
