#pragma once

#include "common_includes.h"
#include "DeviceDependentObject.h"
#include "Setup/Device.h"

class Window;
class Device;
class Surface;

class SwapChain : DeviceDependentObject
{
 public:
    SwapChain() = delete;
    SwapChain(Surface &Surface);
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

    void Recreate();

 private:
    void Create(VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);
    void CreateImageViews();
    VkSurfaceFormatKHR ChooseSurfaceFormat();
    VkPresentModeKHR ChoosePresentMode() const;
    VkExtent2D ChoseExtent(const Surface &Surface) const;

 public:
    Surface &surface;
	SwapChainSupportDetails swapChainSupport;

    VkSwapchainKHR vkSwapChain;

    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;

private:
    VkFormat m_imageFormat;
    VkExtent2D m_resolution;
};