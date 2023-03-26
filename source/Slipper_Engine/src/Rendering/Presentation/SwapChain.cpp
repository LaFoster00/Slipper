#include "SwapChain.h"

#include "Data/Texture/Texture.h"
#include "RenderPass.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"

namespace Slipper
{
VkFormat SwapChain::swapChainFormat = Engine::TARGET_WINDOW_COLOR_FORMAT;

SwapChain::SwapChain(VkExtent2D Extent, VkFormat RenderingFormat)
    : imageRenderingFormat(RenderingFormat),
      imageColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
      depthFormat(Texture2D::FindDepthFormat()),
      resolution(Extent)
{
}

SwapChain::~SwapChain()
{
    Cleanup(false, true);
    depthBuffer.reset();
    renderTarget.reset();
}

void SwapChain::Cleanup(bool KeepRenderPasses, bool CalledFromDestructor)
{
    for (auto &[render_pass, framebuffer] : m_vkFramebuffers) {
        DestroyFramebuffers(render_pass);
    }

    if (!KeepRenderPasses)
        m_vkFramebuffers.clear();

    for (size_t i = 0; i < m_vkImageViews.size(); i++) {
        vkDestroyImageView(device, m_vkImageViews[i], nullptr);
    }

    if (!CalledFromDestructor)
        Impl_Cleanup();
}

void SwapChain::Recreate(uint32_t Width, uint32_t Height)
{
    vkDeviceWaitIdle(Device::Get());
    resolution = {Width, Height};
    Cleanup(true);
    Create();
}

VkImage SwapChain::GetCurrentSwapChainImage() const
{
    return m_vkImages[GetCurrentSwapChainImageIndex()];
}

void SwapChain::Create()
{
    Impl_Create();

    CreateImageViews();
    if (!renderTarget) {
        renderTarget = std::make_unique<RenderTarget>(resolution, imageRenderingFormat);
    }
    else {
        renderTarget->Resize(VkExtent3D(resolution.width, resolution.height, 1));
    }

    if (!depthBuffer) {
        depthBuffer = std::make_unique<DepthBuffer>(resolution, depthFormat);
    }
    else {
        depthBuffer->Resize(VkExtent3D(resolution.width, resolution.height, 1));
    }

    for (const auto render_pass : m_vkFramebuffers) {
        CreateFramebuffers(render_pass.first);
    }
}

void SwapChain::CreateImageViews()
{
    m_vkImageViews.resize(m_vkImages.size());
    for (size_t i = 0; i < m_vkImages.size(); i++) {
        m_vkImageViews[i] = Texture::CreateImageView(
            m_vkImages[i], VK_IMAGE_TYPE_2D, imageRenderingFormat, 1);
    }
}

void SwapChain::CreateFramebuffers(NonOwningPtr<RenderPass> RenderPass)
{
    if (m_vkFramebuffers.contains(RenderPass) && !m_vkFramebuffers.at(RenderPass).empty()) {
        LOG_FORMAT("Destroying old Framebuffers before creating new ones for '{}'",
                   RenderPass->name)
        DestroyFramebuffers(RenderPass);
    }

    auto &vkFramebuffers = m_vkFramebuffers[RenderPass];
    vkFramebuffers.resize(m_vkImageViews.size());
    for (size_t i = 0; i < m_vkImageViews.size(); i++) {
        std::vector<VkImageView> attachments;
        if (GraphicsSettings::Get().MSAA_SAMPLES != VK_SAMPLE_COUNT_1_BIT) {
            attachments.push_back(renderTarget->imageInfo.view);
            attachments.push_back(depthBuffer->imageInfo.view);
            attachments.push_back(m_vkImageViews[i]);
        }
        else {
            attachments.push_back(m_vkImageViews[i]);
            attachments.push_back(depthBuffer->imageInfo.view);
        }
        VkExtent2D extent = GetResolution();

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = RenderPass->vkRenderPass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = extent.width;
        framebuffer_info.height = extent.height;
        framebuffer_info.layers = 1;

        VK_ASSERT(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &vkFramebuffers[i]),
                  "Failed to create framebuffer!")
    }
}

void SwapChain::DestroyFramebuffers(NonOwningPtr<RenderPass> RenderPass)
{
    if (m_vkFramebuffers.contains(RenderPass)) {
        for (auto &framebuffer : m_vkFramebuffers.at(RenderPass)) {
            vkDestroyFramebuffer(Device::Get(), framebuffer, nullptr);
        }
        m_vkFramebuffers.at(RenderPass).clear();
    }
}
}  // namespace Slipper