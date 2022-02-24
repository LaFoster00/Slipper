#pragma once

#include "common_includes.h"
#include <unordered_map>
#include <vector>

class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass
{
public:
    void Create(Device *device, GraphicsPipeline *graphicsPipeline);
    void Destroy();

public:
    Device *owningDevice;

    VkRenderPass vkRenderPass;

    std::unordered_map<const SwapChain *, std::vector<VkFramebuffer>> vkFramebuffers;
};