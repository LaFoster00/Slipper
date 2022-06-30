#pragma once

#include "common_includes.h"

class Device;
class RenderPass;
class SwapChain;

class Framebuffer
{
 public:
    Framebuffer(Device *device,
                RenderPass *renderPass,
                VkImageView *attachments,
                size_t attachmentCount,
                VkExtent2D extent);
    ~Framebuffer();

 private:
    void Create(RenderPass *renderPass,
                VkImageView *attachments,
                size_t attachmentCount,
                VkExtent2D extent);

 public:
    const Device *device;
    VkFramebuffer vkFramebuffer;
};