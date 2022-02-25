#include "RenderPass.h"

#include "../Setup/Device.h"
#include "GraphicsPipeline.h"

#include <algorithm>

RenderPass::RenderPass(Device &device, VkFormat *attachementFormat) : device(device)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = *attachementFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VK_ASSERT(vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &vkRenderPass), "Failed to create render pass")
}

// TODO expand this for more swap chains after introduction of smart pointers
void RenderPass::CreateSwapChainFramebuffers(SwapChain *swapChain)
{
    for (size_t i = 0; i < swapChain->vkImageViews.size(); i++)
    {
        VkImageView *attachments = &swapChain->vkImageViews[i];
        size_t attachmentCount = 1;
        VkExtent2D extent = swapChain->vkExtent;
        framebuffers.emplace_back(&device, this, attachments, attachmentCount, extent);
    }
}

void RenderPass::Destroy()
{
    DestroyAllFrameBuffers();
    vkDestroyRenderPass(device.logicalDevice, vkRenderPass, nullptr);
}

void RenderPass::DestroyAllFrameBuffers()
{
    for (int32_t i = framebuffers.size() - 1; i >= 0; i--)
    {
        DestroyFramebuffer(&framebuffers[i]);
    }
}

/* Destroys the last occurence of the frame buffer. */
void RenderPass::DestroyFramebuffer(Framebuffer *framebuffer)
{
    for (int32_t i = framebuffers.size(); i >= 0; i--)
    {
        if (&framebuffers[i] == framebuffer)
        {
            framebuffers[i].Destroy();
            framebuffers.erase(std::next(framebuffers.begin(), i));
            vkFramebuffers.erase(std::next(vkFramebuffers.begin(), i));
            return;
        }
    }
}
