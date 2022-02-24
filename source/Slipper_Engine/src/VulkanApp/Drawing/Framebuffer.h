#pragma once

#include "common_includes.h"

#include <unordered_map>
#include <vector>

class Device;
class RenderPass;
class SwapChain;

class Framebuffer
{
public:
    Framebuffer(VkImageView *attachment, SwapChain *swapChain, RenderPass *renderPass);
    void Create(VkImageView *attachment, SwapChain *swapChain, RenderPass *renderPass);
    void Destroy();

public:
    const Device *owningDevice;
    const SwapChain *owningSwapchain;
    RenderPass *owningRenderPass;

    VkFramebuffer vkFramebuffer;
};