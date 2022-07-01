#include "SwapChain.h"

#include <algorithm>

#include "common_defines.h"
#include "Surface.h"
#include "Window.h"

SwapChain::SwapChain(Window &window, Surface &surface, bool createViews)
    : device(Device::Get()), swapChainSupport(device.QuerySwapChainSupport(&surface))
{
    const VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat();
    const VkPresentModeKHR presentMode = ChoosePresentMode();
    const VkExtent2D extent = ChoseExtent(window, surface);

	uint32_t imageCount = std::clamp(
        static_cast<uint32_t>(swapChainSupport.capabilities.minImageCount + 1),
        static_cast<uint32_t>(0),
        static_cast<uint32_t>(swapChainSupport.capabilities.maxImageCount));

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t indices[] = {device.queueFamilyIndices.graphicsFamily.value(),
                          device.queueFamilyIndices.presentFamily.value()};

    if (device.queueFamilyIndices.graphicsFamily != device.queueFamilyIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_ASSERT(vkCreateSwapchainKHR(device.logicalDevice, &createInfo, nullptr, &vkSwapChain),
              "Failed to create swap chain!");

	imageCount = 0;
    vkGetSwapchainImagesKHR(device.logicalDevice, vkSwapChain, &imageCount, nullptr);
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.logicalDevice, vkSwapChain, &imageCount, vkImages.data());
    m_imageFormat = createInfo.imageFormat;
    m_resolution = createInfo.imageExtent;

    if (createViews) {
        CreateImageViews();
    }
}

SwapChain::~SwapChain()
{
    for (auto imageView : vkImageViews) {
        vkDestroyImageView(device.logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device.logicalDevice, vkSwapChain, nullptr);
}

void SwapChain::CreateImageViews()
{
    vkImageViews.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = vkImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_imageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_ASSERT(vkCreateImageView(device.logicalDevice, &createInfo, nullptr, &vkImageViews[i]),
                  "Failed to create image views!")
    }
}

VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat()
{
    for (const auto &availableFormat : swapChainSupport.formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return swapChainSupport.formats[0];
}

VkPresentModeKHR SwapChain::ChoosePresentMode()
{
    for (const auto &availablePresentMode : swapChainSupport.presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChoseExtent(Window &window, const Surface &surface)
{
    if (swapChainSupport.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return swapChainSupport.capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window.glfwWindow, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width,
                                        swapChainSupport.capabilities.minImageExtent.width,
                                        swapChainSupport.capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         swapChainSupport.capabilities.minImageExtent.height,
                                         swapChainSupport.capabilities.maxImageExtent.height);

        return actualExtent;
    }
}