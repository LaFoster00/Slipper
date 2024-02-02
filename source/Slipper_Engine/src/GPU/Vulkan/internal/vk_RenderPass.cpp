#include "../vk_RenderPass.h"

namespace Slipper
{
static char8_t ActiveRenderPasses = 0;

RenderPass::RenderPass(std::string_view Name,
                       vk::Format RenderingFormat,
                       vk::Format DepthFormat,
                       bool ForPresentation)
    : name(Name), m_activeSwapChain(nullptr)
{
    // Not used for presenting cause multisampled textures can not be presented
    // Presentation through color attachment resolve
    VkAttachmentDescription color_attachment{};
    color_attachment.format = static_cast<VkFormat>(RenderingFormat);
    color_attachment.samples = static_cast<VkSampleCountFlagBits>(GraphicsSettings::Get().MSAA_SAMPLES);
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else {
        if (ForPresentation)
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        else
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
    }

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Used for presentation
    VkAttachmentDescription color_attachment_resolve{};
    if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
        color_attachment_resolve.format = static_cast<VkFormat>(RenderingFormat);
        color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (ForPresentation)
            color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        else
            color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }

    VkAttachmentReference color_attachment_resolve_ref{};
    if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
        color_attachment_resolve_ref.attachment = 2;
        color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = static_cast<VkFormat>(DepthFormat);
    depth_attachment.samples =  static_cast<VkSampleCountFlagBits>(GraphicsSettings::Get().MSAA_SAMPLES);
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
    if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
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
    if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
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
    vkDestroyRenderPass(device, vkRenderPass, nullptr);
}

void RenderPass::BeginRenderPass(SwapChain *SwapChain,
                                 const uint32_t ImageIndex,
                                 const VkCommandBuffer CommandBuffer)
{
    ActiveRenderPasses++;
    if (!SwapChain->GetVkFramebuffers().contains(this)) {
        ASSERT(1,
               "Swap chain has no framebuffers for this render pass. Did you call "
               "RegisterForRenderPass on RenderingStage for this RenderPass?")
    }
    m_activeSwapChain = SwapChain;

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = vkRenderPass;
    render_pass_info.framebuffer = SwapChain->GetVkFramebuffer(this, ImageIndex);

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
    ActiveRenderPasses--;
    m_activeSwapChain = nullptr;
    vkCmdEndRenderPass(commandBuffer);
}
}  // namespace Slipper