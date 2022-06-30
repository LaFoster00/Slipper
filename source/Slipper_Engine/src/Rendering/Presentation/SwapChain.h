#pragma once

#include "common_includes.h"

class Device;
class Surface;

class SwapChain
{
 public:
    SwapChain() = delete;
    SwapChain(Device &device, VkSwapchainCreateInfoKHR *createInfo, bool createViews);
    ~SwapChain();

 private:
    void CreateImageViews();

 public:
    const Device &device;

    VkSwapchainKHR vkSwapChain;
    VkFormat vkImageFormat;
    VkExtent2D vkExtent;

    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;
};