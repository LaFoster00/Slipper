#include "SwapChain.h"
#include "Device.h"

SwapChain::SwapChain(Device *device, VkSwapchainCreateInfoKHR *createInfo, bool createViews)
{
    owningDevice = device;

    VK_ASSERT(vkCreateSwapchainKHR(device->logicalDevice, createInfo, nullptr, &swapChain), "Failed to create swap chain!");

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device->logicalDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->logicalDevice, swapChain, &imageCount, swapChainImages.data());
    swapChainImageFormat = createInfo->imageFormat;
    swapChainExtent = createInfo->imageExtent;

    if (createViews)
    {
        CreateImageViews();
    }
}

void SwapChain::Destroy()
{
    for (auto imageView : swapChainImageViews)
    {
        vkDestroyImageView(owningDevice->logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(owningDevice->logicalDevice, swapChain, nullptr);
}

void SwapChain::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_ASSERT(
            vkCreateImageView(owningDevice->logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]),
            "Failed to create image views!")
    }
}