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
    static std::unordered_map<const SwapChain *, std::vector<VkFramebuffer>> VkFramebuffers;

public:
    const Device *owningDevice;
    const SwapChain *owningSwapchain;
    VkFramebuffer vkFramebuffer;
};