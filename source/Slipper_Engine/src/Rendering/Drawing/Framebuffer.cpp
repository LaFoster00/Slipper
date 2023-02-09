#include "Framebuffer.h"

#include "GraphicsPipeline/RenderPass.h"
#include "Setup/Device.h"
#include "common_defines.h"

#include <algorithm>

namespace Slipper
{
Framebuffer::Framebuffer(const RenderPass *RenderPass,
                         const VkImageView *Attachments,
                         const uint32_t AttachmentCount,
                         const VkExtent2D Extent)
{
    Create(RenderPass, Attachments, AttachmentCount, Extent);
}

void Framebuffer::Create(const RenderPass *RenderPass,
                         const VkImageView *Attachments,
                         const uint32_t AttachmentCount,
                         const VkExtent2D Extent)
{
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = RenderPass->vkRenderPass;
    framebuffer_info.attachmentCount = AttachmentCount;
    framebuffer_info.pAttachments = Attachments;
    framebuffer_info.width = Extent.width;
    framebuffer_info.height = Extent.height;
    framebuffer_info.layers = 1;

    VK_ASSERT(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &vkFramebuffer),
              "Failed to create framebuffer!")
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(device, vkFramebuffer, nullptr);
}
}  // namespace Slipper