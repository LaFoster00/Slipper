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

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_ASSERT(vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &vkRenderPass), "Failed to create render pass")
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

// TODO: expand for more swap chains after smart pointers
void RenderPass::BeginRenderPass(SwapChain *swapChain, uint32_t imageIndex, VkCommandBuffer commandBuffer, GraphicsPipeline &graphicsPipeline)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkRenderPass;
    renderPassInfo.framebuffer = vkFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->vkExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.vkGraphicsPipeline);
}

void RenderPass::EndRenderPass(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}