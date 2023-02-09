#include "SwapChain.h"

#include <algorithm>

#include "Data/Texture/Texture.h"
#include "Window.h"
#include "common_defines.h"
#include "Surface.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"

namespace Slipper
{
VkFormat SwapChain::swapChainFormat = Engine::TARGET_COLOR_FORMAT;

SwapChain::SwapChain(Surface &Surface) : surface(Surface)
{
    Create();
}

SwapChain::~SwapChain()
{
    for (const auto image_view : vkImageViews) {
        vkDestroyImageView(device.logicalDevice, image_view, nullptr);
    }

    depthBuffer.reset();

    vkDestroySwapchainKHR(device.logicalDevice, vkSwapChain, nullptr);
}

void SwapChain::Recreate()
{
    for (const auto image_view : vkImageViews) {
        vkDestroyImageView(device.logicalDevice, image_view, nullptr);
    }
    vkImageViews.clear();
    const VkSwapchainKHR old_swap_chain = vkSwapChain;

    Create(old_swap_chain);

    vkDestroySwapchainKHR(device, old_swap_chain, nullptr);
}

void SwapChain::Create(VkSwapchainKHR oldSwapChain)
{
    swapChainSupport = device.QuerySwapChainSupport(&surface);

    const auto [format, colorSpace] = ChooseSurfaceFormat();
    const VkPresentModeKHR present_mode = ChoosePresentMode();
    const VkExtent2D extent = ChoseExtent(surface);

    uint32_t image_count = std::clamp(
        static_cast<uint32_t>(swapChainSupport.capabilities.minImageCount + 1),
        static_cast<uint32_t>(0),
        static_cast<uint32_t>(swapChainSupport.capabilities.maxImageCount));

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface.surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format;
    create_info.imageColorSpace = colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t indices[] = {device.queueFamilyIndices.graphicsFamily.value(),
                                device.queueFamilyIndices.presentFamily.value()};

    if (device.queueFamilyIndices.graphicsFamily != device.queueFamilyIndices.presentFamily) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = indices;
    }
    else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;      // Optional
        create_info.pQueueFamilyIndices = nullptr;  // Optional
    }

    create_info.preTransform = swapChainSupport.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = oldSwapChain;

    VK_ASSERT(vkCreateSwapchainKHR(device, &create_info, nullptr, &vkSwapChain),
              "Failed to create swap chain!");

    image_count = 0;
    vkGetSwapchainImagesKHR(device, vkSwapChain, &image_count, nullptr);
    vkImages.resize(image_count);
    vkGetSwapchainImagesKHR(device, vkSwapChain, &image_count, vkImages.data());
    m_imageFormat = create_info.imageFormat;
    m_resolution = create_info.imageExtent;

    CreateImageViews();

    if (!renderTarget) {
        renderTarget = std::make_unique<RenderTarget>(extent, format);
    }
    else {
        renderTarget->Resize(VkExtent3D(extent.width, extent.height, 1));
    }

    if (!depthBuffer) {
        m_depthFormat = Texture2D::FindDepthFormat();
        depthBuffer = std::make_unique<DepthBuffer>(extent, m_depthFormat);
    }
    else {
        depthBuffer->Resize(VkExtent3D(extent.width, extent.height, 1));
    }
}

void SwapChain::CreateImageViews()
{
    vkImageViews.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); i++) {
        vkImageViews[i] = Texture::CreateImageView(
            vkImages[i], VK_IMAGE_TYPE_2D, m_imageFormat, 1);
    }
}

VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat() const
{
    for (const auto &available_format : swapChainSupport.formats) {
        if (available_format.format == Engine::TARGET_COLOR_FORMAT &&
            available_format.colorSpace == Engine::TARGET_COLOR_SPACE) {
            swapChainFormat = Engine::TARGET_COLOR_FORMAT;
            return available_format;
        }
    }

    swapChainFormat = swapChainSupport.formats[0].format;
    return swapChainSupport.formats[0];
}

VkPresentModeKHR SwapChain::ChoosePresentMode() const
{
    for (const auto &available_present_mode : swapChainSupport.presentModes) {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChoseExtent(const Surface &Surface) const
{
    if (swapChainSupport.capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return swapChainSupport.capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(Surface.window, &width, &height);

        VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actual_extent.width = std::clamp(actual_extent.width,
                                         swapChainSupport.capabilities.minImageExtent.width,
                                         swapChainSupport.capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height,
                                          swapChainSupport.capabilities.minImageExtent.height,
                                          swapChainSupport.capabilities.maxImageExtent.height);

        return actual_extent;
    }
}
}  // namespace Slipper