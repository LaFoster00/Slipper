#include "SwapChain.h"

#include <algorithm>

#include "Surface.h"
#include "common_defines.h"
#include "Window.h"

SwapChain::SwapChain(Surface &Surface) : surface(Surface)
{
    Create();
}

SwapChain::~SwapChain()
{
    for (const auto image_view : vkImageViews) {
        vkDestroyImageView(device.logicalDevice, image_view, nullptr);
    }

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
}

void SwapChain::CreateImageViews()
{
    vkImageViews.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); i++) {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = vkImages[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_imageFormat;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        VK_ASSERT(vkCreateImageView(device, &create_info, nullptr, &vkImageViews[i]),
                  "Failed to create image views!")
    }
}

VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat()
{
    for (const auto &available_format : swapChainSupport.formats) {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }

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