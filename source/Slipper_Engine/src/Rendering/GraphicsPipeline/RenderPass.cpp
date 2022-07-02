#include "RenderPass.h"

#include "../Setup/Device.h"
#include "GraphicsPipeline.h"

#include <algorithm>

#include "common_defines.h"
#include "Presentation/SwapChain.h"

RenderPass::RenderPass(Device &device, VkFormat attachmentFormat) : device(device)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = attachmentFormat;
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

    VK_ASSERT(vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &vkRenderPass),
              "Failed to create render pass")
}

RenderPass::~RenderPass()
{
    DestroyAllFrameBuffers();
    vkDestroyRenderPass(device.logicalDevice, vkRenderPass, nullptr);
}

void RenderPass::DestroyAllFrameBuffers()
{
    swapChainFramebuffers.clear();
}

/* Destroys the last occurence of the frame buffer. */
bool RenderPass::DestroySwapChainFramebuffers(SwapChain *SwapChain)
{
    if (swapChainFramebuffers.contains(SwapChain)) {
        swapChainFramebuffers.erase(SwapChain);
        return true;
    }
    return false;
}

void RenderPass::CreateSwapChainFramebuffers(SwapChain *swapChain)
{
    if (swapChainFramebuffers.contains(swapChain)) {
        ASSERT(1,
               "Swapchain allready has Framebuffers for this swap chain. Clean them up before "
               "creating new ones!")
    }
    for (size_t i = 0; i < swapChain->vkImageViews.size(); i++) {
        VkImageView *attachments = &swapChain->vkImageViews[i];
        size_t attachmentCount = 1;
        VkExtent2D extent = swapChain->GetResolution();
        swapChainFramebuffers[swapChain].emplace_back(
            std::make_unique<Framebuffer>(this, attachments, attachmentCount, extent));
    }
}

void RenderPass::BeginRenderPass(SwapChain *swapChain,
                                 uint32_t imageIndex,
                                 VkCommandBuffer commandBuffer)
{
    m_activeSwapChain = swapChain;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkRenderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers.at(swapChain)[imageIndex]->vkFramebuffer;

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->GetResolution();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::EndRenderPass(VkCommandBuffer commandBuffer)
{
    m_activeSwapChain = nullptr;
    vkCmdEndRenderPass(commandBuffer);
}

void RenderPass::RegisterShader(Shader* Shader)
{
    registeredShaders.insert(Shader);
}

void RenderPass::UnregisterShader(Shader* Shader)
{
    if (registeredShaders.contains(Shader))
    {
        registeredShaders.erase(Shader);
    }
}
