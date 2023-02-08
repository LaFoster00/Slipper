#pragma once
#include "Texture2D.h"

class RenderTarget : public Texture
{
public:
    // MSAA Settings set in Engine::Settings::MSAA_SAMPLES
    RenderTarget(
            VkExtent2D Extent,
            VkFormat Format);
};