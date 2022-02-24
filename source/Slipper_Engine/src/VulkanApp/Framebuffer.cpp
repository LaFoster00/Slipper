#include "Framebuffer.h"

#include "Device.h"
#include "RenderPass.h"
#include "common_defines.h"
#include <algorithm>

std::unordered_map<const SwapChain *, std::vector<VkFramebuffer>> Framebuffer::VkFramebuffers;

Framebuffer::Framebuffer(VkImageView *attachment, SwapChain *swapChain, RenderPass *renderPass)
{
    Create(attachment, swapChain, renderPass);
}

void Framebuffer::Create(VkImageView *attachments, SwapChain *swapChain, RenderPass *renderPass)
{
    owningDevice = swapChain->owningDevice;
    owningSwapchain = swapChain;

    if (!VkFramebuffers.contains(swapChain))
    {
        VkFramebuffers.emplace(swapChain, (swapChain->swapChainImageViews.size()));
    }
    else
    {
        VkFramebuffers[swapChain].resize(swapChain->swapChainImageViews.size());
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChain->swapChainExtent.width;
    framebufferInfo.height = swapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    VkFramebuffer *framebuffers = VkFramebuffers[swapChain].data();
    VK_ASSERT(vkCreateFramebuffer(swapChain->owningDevice->logicalDevice, &framebufferInfo, nullptr, framebuffers), "Failed to create framebuffer!")
}

void Framebuffer::Destroy()
{
    std::vector<VkFramebuffer> &framebuffers = VkFramebuffers[owningSwapchain];
    auto itr = std::find(framebuffers.begin(), framebuffers.end(), vkFramebuffer);

    if (itr == framebuffers.end())
    {
        std::cout << "Dont delete framebuffers multiple times!\n";
        return;
    }

    framebuffers.erase(itr);

    vkDestroyFramebuffer(owningDevice->logicalDevice, vkFramebuffer, nullptr);
}