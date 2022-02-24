#include "Framebuffer.h"

#include "../Setup/Device.h"
#include "../GraphicsPipeline/RenderPass.h"
#include "common_defines.h"
#include <algorithm>

Framebuffer::Framebuffer(VkImageView *attachment, SwapChain *swapChain, RenderPass *renderPass)
{
    Create(attachment, swapChain, renderPass);
}

void Framebuffer::Create(VkImageView *attachments, SwapChain *swapChain, RenderPass *renderPass)
{
    owningDevice = swapChain->owningDevice;
    owningSwapchain = swapChain;
    owningRenderPass = renderPass;

    if (!renderPass->vkFramebuffers.contains(swapChain))
    {
        renderPass->vkFramebuffers.emplace(swapChain, (swapChain->swapChainImageViews.size()));
    }
    else
    {
        renderPass->vkFramebuffers[swapChain].resize(swapChain->swapChainImageViews.size(), VK_NULL_HANDLE);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChain->swapChainExtent.width;
    framebufferInfo.height = swapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    /* Find the next free framebuffer in the array. */
    VkFramebuffer *framebuffer;
    for (size_t i = 0; i < renderPass->vkFramebuffers[swapChain].size(); i++)
    {
        if (renderPass->vkFramebuffers[swapChain][i] == VK_NULL_HANDLE)
        {
            framebuffer = &renderPass->vkFramebuffers[swapChain][i];
        }
    }
    VK_ASSERT(vkCreateFramebuffer(swapChain->owningDevice->logicalDevice, &framebufferInfo, nullptr, framebuffer), "Failed to create framebuffer!")
    vkFramebuffer = *framebuffer;
}

void Framebuffer::Destroy()
{
    std::vector<VkFramebuffer> &framebuffers = owningRenderPass->vkFramebuffers[owningSwapchain];
    auto itr = std::find(framebuffers.begin(), framebuffers.end(), vkFramebuffer);

    if (itr == framebuffers.end())
    {
        std::cout << "Dont delete framebuffers multiple times!\n";
        return;
    }

    framebuffers.erase(itr);

    vkDestroyFramebuffer(owningDevice->logicalDevice, vkFramebuffer, nullptr);
}