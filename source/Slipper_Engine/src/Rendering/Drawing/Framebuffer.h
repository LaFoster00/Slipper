#pragma once

#include "DeviceDependentObject.h"
#include "common_includes.h"

class Device;
class RenderPass;
class SwapChain;

class Framebuffer : DeviceDependentObject
{
 public:
    Framebuffer(const RenderPass *RenderPass,
                const VkImageView *Attachments,
                size_t AttachmentCount,
                VkExtent2D Extent);
    ~Framebuffer();

 private:
    void Create(const RenderPass *RenderPass,
                const VkImageView *Attachments,
                size_t AttachmentCount,
                VkExtent2D Extent);

 public:
    VkFramebuffer vkFramebuffer;
};