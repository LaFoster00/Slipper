#include "SwapChain.h"

#include "Data/Texture/Texture.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"

namespace Slipper
{
VkFormat SwapChain::swapChainFormat = Engine::TARGET_WINDOW_COLOR_FORMAT;

SwapChain::SwapChain(VkExtent2D Extent,
                     VkFormat RenderingFormat)
    : vkSwapChain(nullptr),
      imageRenderingFormat(RenderingFormat),
      imageColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
      depthFormat(Texture2D::FindDepthFormat()),
      resolution(Extent)
{
}

SwapChain::~SwapChain()
{
    // Call ClearImages in derived desctructors for correct behaviour
    depthBuffer.reset();
    renderTarget.reset();
}

void SwapChain::ClearImages()
{
    for (const auto image_view : m_vkImageViews) {
        vkDestroyImageView(device, image_view, nullptr);
    }
    m_vkImageViews.clear();
}

void SwapChain::Recreate(uint32_t Width, uint32_t Height)
{
    vkDeviceWaitIdle(device);

    ClearImages();
    const VkSwapchainKHR old_swap_chain = vkSwapChain;

    resolution = {Width, Height};
    Create(old_swap_chain);

    vkDestroySwapchainKHR(device, old_swap_chain, nullptr);
}

VkImage SwapChain::GetCurrentSwapChainImage() const
{
	return m_vkImages[GetCurrentSwapChainImageIndex()];
}

void SwapChain::Create(VkSwapchainKHR OldSwapChain)
{
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
}

void SwapChain::CreateImageViews()
{
    m_vkImageViews.resize(m_vkImages.size());
    for (size_t i = 0; i < m_vkImages.size(); i++) {
        m_vkImageViews[i] = Texture::CreateImageView(
            m_vkImages[i], VK_IMAGE_TYPE_2D, imageRenderingFormat, 1);
    }
}
}  // namespace Slipper