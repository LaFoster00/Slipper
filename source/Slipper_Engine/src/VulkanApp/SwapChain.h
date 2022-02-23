#pragma once

#include "common_includes.h"
#include "common_defines.h"

class Device;
class Surface;

class SwapChain
{
public:
    SwapChain() = delete;
    SwapChain(Device *device, VkSwapchainCreateInfoKHR *createInfo, bool createViews);
    void Destroy();

    void CreateImageViews();

public:
    const Device *owningDevice;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> swapChainImageViews;
};