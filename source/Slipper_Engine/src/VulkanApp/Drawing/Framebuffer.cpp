#include "Framebuffer.h"

#include "../Setup/Device.h"
#include "../GraphicsPipeline/RenderPass.h"
#include "common_defines.h"
#include <algorithm>

Framebuffer::Framebuffer(Device *device, RenderPass *renderPass, VkImageView *attachments, size_t attachmentCount, VkExtent2D extent)
{
    this->device = device;
    Create(renderPass, attachments, attachmentCount, extent);
}

void Framebuffer::Create(RenderPass *renderPass, VkImageView *attachments, size_t attachmentCount, VkExtent2D extent)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    /* Find the next free framebuffer in the array. */
    VkFramebuffer *framebuffer = VK_NULL_HANDLE;
    framebuffer = &renderPass->vkFramebuffers.emplace_back(VK_NULL_HANDLE);

    VK_ASSERT(vkCreateFramebuffer(device->logicalDevice, &framebufferInfo, nullptr, framebuffer), "Failed to create framebuffer!")
    vkFramebuffer = *framebuffer;
}

void Framebuffer::Destroy()
{
    const Device *deviceptr = device;
    vkDestroyFramebuffer(device->logicalDevice, vkFramebuffer, nullptr);
}