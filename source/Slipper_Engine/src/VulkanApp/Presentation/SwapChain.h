#pragma once

#include "common_includes.h"
#include "common_defines.h"

class Device;
class Surface;

class SwapChain
{
public:
    SwapChain() = delete;
    SwapChain(Device &device, VkSwapchainCreateInfoKHR *createInfo, bool createViews);
    void Destroy();

    void CreateImageViews();

public:
    const Device &device;

    VkSwapchainKHR vkSwapChain;
    VkFormat vkImageFormat;
    VkExtent2D vkExtent;

    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;
};