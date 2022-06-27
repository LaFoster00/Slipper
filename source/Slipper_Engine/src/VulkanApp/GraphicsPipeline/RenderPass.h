#pragma once

#include "common_includes.h"
#include "../Drawing/Framebuffer.h"

#include <unordered_map>
#include <vector>

class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass
{
public:
    RenderPass() = delete;
    RenderPass(Device &device, VkFormat *attachementFormat);
    void Destroy();

    void DestroyAllFrameBuffers();
    void DestroyFramebuffer(Framebuffer *framebuffer);

    void CreateSwapChainFramebuffers(SwapChain *swapChain);

    void BeginRenderPass(SwapChain *swapChain, uint32_t imageIndex, VkCommandBuffer commandBuffer, GraphicsPipeline &graphicsPipeline);
    void EndRenderPass(VkCommandBuffer commandBuffer);

public:
    Device &device;

    VkRenderPass vkRenderPass;

    std::vector<Framebuffer> framebuffers;
};