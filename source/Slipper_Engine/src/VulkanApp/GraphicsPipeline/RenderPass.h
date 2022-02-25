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

public:
    Device &device;

    VkRenderPass vkRenderPass;

    std::vector<Framebuffer> framebuffers;
    std::vector<VkFramebuffer> vkFramebuffers;
};