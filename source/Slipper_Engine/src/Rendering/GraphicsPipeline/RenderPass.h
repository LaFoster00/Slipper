#pragma once

#include "Drawing/Framebuffer.h"
#include "common_includes.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass
{
 public:
    RenderPass() = delete;
    RenderPass(Device &device, VkFormat attachementFormat);
    ~RenderPass();

    void DestroyAllFrameBuffers();
    bool DestroySwapChainFramebuffers(SwapChain *SwapChain);

    void CreateSwapChainFramebuffers(SwapChain *swapChain);

    void BeginRenderPass(SwapChain *swapChain, uint32_t imageIndex, VkCommandBuffer commandBuffer);
    void EndRenderPass(VkCommandBuffer commandBuffer);

 public:
    Device &device;

    VkRenderPass vkRenderPass;

    std::unordered_map<SwapChain *, std::vector<std::unique_ptr<Framebuffer>>>
        swapChainFramebuffers;
};