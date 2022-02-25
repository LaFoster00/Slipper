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
    Framebuffer(Device *device, RenderPass *renderPass, VkImageView *attachments, size_t attachmentCount, VkExtent2D extent);
    void Create(RenderPass *renderPass, VkImageView *attachments, size_t attachmentCount, VkExtent2D extent);
    void Destroy();

public:
    const Device *device;
    VkFramebuffer vkFramebuffer;
};