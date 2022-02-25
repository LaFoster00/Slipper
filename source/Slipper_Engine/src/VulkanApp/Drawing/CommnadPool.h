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
    void BeginCommandBuffer(RenderPass *renderpass, SwapChain *swapChain, VkCommandBuffer commandBuffer, uint32_t imageIndex);

public:
    Device &owningDevice;

    VkCommandPool vkCommandPool;

    std::vector<VkCommandBuffer> vkCommandBuffers;
};