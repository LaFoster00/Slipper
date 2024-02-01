#include "SwapChain.h"

#include "Data/Texture/Texture.h"
#include "RenderPass.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"

namespace Slipper
{
SwapChain::SwapChain(vk::Extent2D Extent, vk::Format RenderingFormat)
    : imageRenderingFormat(RenderingFormat),
      imageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear),
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
    for (const auto &render_pass : m_vkFramebuffers | std::views::keys) {
        DestroyFramebuffers(render_pass);
    }

    if (!KeepRenderPasses)
        m_vkFramebuffers.clear();

    for (size_t i = 0; i < m_vkImageViews.size(); i++) {
        device.logicalDevice.destroyImageView(m_vkImageViews[i]);
    }
    m_vkImageViews.clear();

    if (!CalledFromDestructor)
        Impl_Cleanup();
}

void SwapChain::Recreate(uint32_t Width, uint32_t Height)
{
    device.logicalDevice.waitIdle();
    resolution.width = Width;
    resolution.height = Height;

    Cleanup(true);
    Create();
}

vk::Image SwapChain::GetCurrentSwapChainImage() const
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

    for (const auto render_pass : m_vkFramebuffers | std::views::keys) {
        CreateFramebuffers(render_pass);
    }
}

void SwapChain::CreateImageViews()
{
    m_vkImageViews.resize(m_vkImages.size());
    for (size_t i = 0; i < m_vkImages.size(); i++) {
        m_vkImageViews[i] = Texture::CreateImageView(
            m_vkImages[i], vk::ImageType::e2D, imageRenderingFormat, 1);
    }
}

void SwapChain::CreateFramebuffers(NonOwningPtr<RenderPass> RenderPass)
{
    if (m_vkFramebuffers.contains(RenderPass) && !m_vkFramebuffers.at(RenderPass).empty()) {
        LOG_FORMAT("Destroying old Framebuffers before creating new ones for '{}'",
                   RenderPass->name)
        DestroyFramebuffers(RenderPass);
    }

    auto &vk_framebuffers = m_vkFramebuffers[RenderPass];
    vk_framebuffers.resize(m_vkImageViews.size());
    for (size_t i = 0; i < m_vkImageViews.size(); i++) {
        std::vector<vk::ImageView> attachments;
        if (GraphicsSettings::Get().MSAA_SAMPLES != vk::SampleCountFlagBits::e1) {
            attachments.push_back(renderTarget->imageInfo.views[0]);
            attachments.push_back(depthBuffer->imageInfo.views[0]);
            attachments.push_back(m_vkImageViews[i]);
        }
        else {
            attachments.push_back(m_vkImageViews[i]);
            attachments.push_back(depthBuffer->imageInfo.views[0]);
        }
        const vk::Extent2D extent = GetResolution();

        vk::FramebufferCreateInfo framebuffer_info(
            {}, RenderPass->vkRenderPass, attachments, extent.width, extent.height, 1);

        VK_HPP_ASSERT(device.logicalDevice.createFramebuffer(
                          &framebuffer_info, nullptr, &vk_framebuffers[i]),
                      "Failed to create framebuffer!")
    }
}

void SwapChain::DestroyFramebuffers(NonOwningPtr<RenderPass> RenderPass)
{
    if (m_vkFramebuffers.contains(RenderPass)) {
        for (const auto &framebuffer : m_vkFramebuffers.at(RenderPass)) {
            device.logicalDevice.destroyFramebuffer(framebuffer);
        }
        m_vkFramebuffers.at(RenderPass).clear();
    }
}
}  // namespace Slipper