#pragma once

#include "common_includes.h"
#include "Setup/Device.h"

class Window;
class Device;
class Surface;

class SwapChain
{
 public:
    SwapChain() = delete;
    SwapChain(Window &window, Surface &surface, bool createViews);
    ~SwapChain();

    [[nodiscard]] const VkExtent2D &GetResolution() const
	{
        return m_resolution;
	}

    [[nodiscard]] const VkFormat &GetFormat() const
    {
        return m_imageFormat;
	}

    operator VkSwapchainKHR() const
    {
        return vkSwapChain;
	}

 private:
    void CreateImageViews();
    VkSurfaceFormatKHR ChooseSurfaceFormat();
    VkPresentModeKHR ChoosePresentMode();
    VkExtent2D ChoseExtent(Window &window, const Surface &surface);

 public:
	Device &device;
	SwapChainSupportDetails swapChainSupport;

    VkSwapchainKHR vkSwapChain;

    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;

private:
    VkFormat m_imageFormat;
    VkExtent2D m_resolution;
};