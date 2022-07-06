#include "RenderPass.h"

#include "../Setup/Device.h"
#include "GraphicsPipeline.h"

#include <algorithm>

#include "common_defines.h"
#include "Presentation/SwapChain.h"

RenderPass::RenderPass(VkFormat AttachmentFormat)
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = AttachmentFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VK_ASSERT(vkCreateRenderPass(device, &render_pass_info, nullptr, &vkRenderPass),
              "Failed to create render pass")
}

RenderPass::~RenderPass()
{
    DestroyAllFrameBuffers();
    vkDestroyRenderPass(device, vkRenderPass, nullptr);
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

void RenderPass::CreateSwapChainFramebuffers(SwapChain *SwapChain)
{
    if (swapChainFramebuffers.contains(SwapChain)) {
        ASSERT(1,
               "Swapchain allready has Framebuffers for this swap chain. Clean them up before "
               "creating new ones!")
    }
    for (size_t i = 0; i < SwapChain->vkImageViews.size(); i++) {
        VkImageView *attachments = &SwapChain->vkImageViews[i];
        size_t attachment_count = 1;
        VkExtent2D extent = SwapChain->GetResolution();
        swapChainFramebuffers[SwapChain].emplace_back(
            std::make_unique<Framebuffer>(this, attachments, attachment_count, extent));
    }
}

void RenderPass::BeginRenderPass(SwapChain *SwapChain,
                                 const uint32_t ImageIndex,
                                 const VkCommandBuffer CommandBuffer)
{
    m_activeSwapChain = SwapChain;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkRenderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers.at(SwapChain)[ImageIndex]->vkFramebuffer;

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = SwapChain->GetResolution();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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
