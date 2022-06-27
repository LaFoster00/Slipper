#pragma once

#include "common_includes.h"
#include "common_defines.h"
#include "Framebuffer.h"

class Device;
class RenderPass;

class CommandPool
{
public:
    CommandPool() = delete;
    CommandPool(Device &device);
    void Destroy();

    VkCommandBuffer CreateCommandBuffer();
    VkCommandBuffer BeginCommandBuffer(uint32_t bufferIndex, bool resetCommandBuffer = true);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);
    void EndCommandBuffer(uint32_t bufferIndex);

public:
    Device &owningDevice;

    VkCommandPool vkCommandPool;

    std::vector<VkCommandBuffer> vkCommandBuffers;
};