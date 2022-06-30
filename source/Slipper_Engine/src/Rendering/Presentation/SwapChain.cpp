#include "SwapChain.h"

#include "common_defines.h"
#include "../Setup/Device.h"

SwapChain::SwapChain(Device &device, VkSwapchainCreateInfoKHR *createInfo, bool createViews)
    : device(device)
{
    VK_ASSERT(vkCreateSwapchainKHR(device.logicalDevice, createInfo, nullptr, &vkSwapChain),
              "Failed to create swap chain!");

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device.logicalDevice, vkSwapChain, &imageCount, nullptr);
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.logicalDevice, vkSwapChain, &imageCount, vkImages.data());
    vkImageFormat = createInfo->imageFormat;
    vkExtent = createInfo->imageExtent;

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
        createInfo.format = vkImageFormat;

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