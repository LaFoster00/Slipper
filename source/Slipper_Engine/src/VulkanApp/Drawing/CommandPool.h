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
    CommandPool(Device &device, int32_t BufferCount);
    ~CommandPool();
    
    VkCommandBuffer BeginCommandBuffer(uint32_t bufferIndex, bool resetCommandBuffer = true);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);
    void EndCommandBuffer(uint32_t bufferIndex);

private:
    std::vector<VkCommandBuffer>& CreateCommandBuffers(int32_t BufferCount);

public:
    Device &owningDevice;

    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> vkCommandBuffers;
};