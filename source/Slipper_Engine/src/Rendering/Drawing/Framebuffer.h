#pragma once

#include "common_includes.h"
#include "DeviceDependentObject.h"

class Device;
class RenderPass;
class SwapChain;

class Framebuffer : DeviceDependentObject
{
 public:
    Framebuffer(RenderPass *renderPass,
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
    VkFramebuffer vkFramebuffer;
};