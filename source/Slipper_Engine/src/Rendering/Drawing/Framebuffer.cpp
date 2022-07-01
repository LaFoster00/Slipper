#include "Framebuffer.h"

#include "common_defines.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Setup/Device.h"

#include <algorithm>

Framebuffer::Framebuffer(RenderPass *renderPass,
                         VkImageView *attachments,
                         size_t attachmentCount,
                         VkExtent2D extent)
    : DeviceDependentObject()
{
    Create(renderPass, attachments, attachmentCount, extent);
}

void Framebuffer::Create(RenderPass *renderPass,
                         VkImageView *attachments,
                         size_t attachmentCount,
                         VkExtent2D extent)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VK_ASSERT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vkFramebuffer),
              "Failed to create framebuffer!")
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(device, vkFramebuffer, nullptr);
}