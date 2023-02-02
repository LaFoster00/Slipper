#pragma once

#include <memory>

#include "DeviceDependentObject.h"
#include "Setup/Device.h"

class DepthBuffer;
class RenderTarget;
class Texture2D;
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

    [[nodiscard]] const VkFormat &GetImageFormat() const
    {
        return m_imageFormat;
	}

    [[nodiscard]] const VkFormat &GetDepthFormat() const
    {
        return m_depthFormat;
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

    std::unique_ptr<RenderTarget> renderTarget;
    std::unique_ptr<DepthBuffer> depthBuffer;

private:
    VkFormat m_imageFormat;
    VkFormat m_depthFormat;
    VkExtent2D m_resolution;
};