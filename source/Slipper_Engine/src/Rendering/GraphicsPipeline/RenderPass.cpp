#include "RenderPass.h"

#include "../Setup/Device.h"

#include <algorithm>
#include <array>

#include "Drawing/Framebuffer.h"
#include "Presentation/SwapChain.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"
#include "Texture/Texture2D.h"
#include "common_defines.h"
#include "Setup/GraphicsSettings.h"

RenderPass::RenderPass(VkFormat AttachmentFormat, VkFormat DepthFormat)
    : m_activeSwapChain(nullptr)
{
    // Not used for presenting cause multisampled textures can not be presented
    // Presentation through color attachment resolve
    VkAttachmentDescription color_attachment{};
    color_attachment.format = AttachmentFormat;
    color_attachment.samples = GraphicsSettings::Get().MSAA_SAMPLES;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else {
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Used for presentation
    VkAttachmentDescription color_attachment_resolve{};
    if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
        color_attachment_resolve.format = AttachmentFormat;
        color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    VkAttachmentReference color_attachment_resolve_ref{};
    if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
        color_attachment_resolve_ref.attachment = 2;
        color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = DepthFormat;
    depth_attachment.samples = GraphicsSettings::Get().MSAA_SAMPLES;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
        subpass.pResolveAttachments = &color_attachment_resolve_ref;
    }

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::vector attachments = {color_attachment, depth_attachment};
    if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
        attachments.push_back(color_attachment_resolve);
    }
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
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
        std::vector<VkImageView> attachments;
        if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
            attachments.push_back(SwapChain->renderTarget->textureView);
            attachments.push_back(SwapChain->depthBuffer->textureView);
            attachments.push_back(SwapChain->vkImageViews[i]);
        }
        else {
            attachments.push_back(SwapChain->vkImageViews[i]);
            attachments.push_back(SwapChain->depthBuffer->textureView);
        }
        VkExtent2D extent = SwapChain->GetResolution();
        swapChainFramebuffers[SwapChain].emplace_back(std::make_unique<Framebuffer>(
            this, attachments.data(), static_cast<uint32_t>(attachments.size()), extent));
    }
}

void RenderPass::BeginRenderPass(SwapChain *SwapChain,
                                 const uint32_t ImageIndex,
                                 const VkCommandBuffer CommandBuffer)
{
    m_activeSwapChain = SwapChain;

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = vkRenderPass;
    render_pass_info.framebuffer = swapChainFramebuffers.at(SwapChain)[ImageIndex]->vkFramebuffer;

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = SwapChain->GetResolution();

    std::array<VkClearValue, 2> clear_color{};
    clear_color[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_color[1].depthStencil = {1.0f, 0};

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_color.size());
    render_pass_info.pClearValues = clear_color.data();

    vkCmdBeginRenderPass(CommandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::EndRenderPass(VkCommandBuffer commandBuffer)
{
    m_activeSwapChain = nullptr;
    vkCmdEndRenderPass(commandBuffer);
}

void RenderPass::RegisterShader(Shader *Shader)
{
    registeredShaders.insert(Shader);
}

void RenderPass::UnregisterShader(Shader *Shader)
{
    if (registeredShaders.contains(Shader)) {
        registeredShaders.erase(Shader);
    }
}
