#include "SwapChain.h"

#include <algorithm>

#include "Data/Texture/Texture.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"
#include "Window.h"
#include "common_defines.h"

namespace Slipper
{
VkFormat SwapChain::swapChainFormat = Engine::TARGET_WINDOW_COLOR_FORMAT;

SwapChain::SwapChain(VkExtent2D Extent, VkFormat Format)
    : vkSwapChain(nullptr),
      imageFormat(Format),
      imageColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
      depthFormat(Texture2D::FindDepthFormat()),
      resolution(Extent)
{
}

SwapChain::~SwapChain()
{
    for (const auto image_view : vkImageViews) {
        vkDestroyImageView(device, image_view, nullptr);
    }
    vkImageViews.clear();

    depthBuffer.reset();
    renderTarget.reset();
}

void SwapChain::Recreate(uint32_t Width, uint32_t Height)
{
    for (const auto image_view : vkImageViews) {
        vkDestroyImageView(device, image_view, nullptr);
    }
    vkImageViews.clear();
    const VkSwapchainKHR old_swap_chain = vkSwapChain;

    resolution = {Width, Height};
    Create(old_swap_chain);

    vkDestroySwapchainKHR(device, old_swap_chain, nullptr);
}

void SwapChain::Create(VkSwapchainKHR OldSwapChain)
{
    CreateImageViews();

    if (!renderTarget) {
        renderTarget = std::make_unique<RenderTarget>(resolution, imageFormat);
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
    vkImageViews.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); i++) {
        vkImageViews[i] = Texture::CreateImageView(vkImages[i], VK_IMAGE_TYPE_2D, imageFormat, 1);
    }
}
}  // namespace Slipper