#pragma once

#include <memory>

#include "DeviceDependentObject.h"
#include "Setup/Device.h"

namespace Slipper
{
class DepthBuffer;
class RenderTarget;
class Texture2D;
class Window;
class Device;
class Surface;

class SwapChain : public DeviceDependentObject
{
 public:
    SwapChain() = delete;
    virtual ~SwapChain();

    operator VkSwapchainKHR() const
    {
        return vkSwapChain;
    }

    [[nodiscard]] const VkExtent2D &GetResolution() const
    {
        return resolution;
    }

    [[nodiscard]] const VkFormat &GetImageFormat() const
    {
        return imageFormat;
    }

    [[nodiscard]] const VkFormat &GetDepthFormat() const
    {
        return depthFormat;
    }

    void Recreate(uint32_t Width, uint32_t Height);

 protected:
    SwapChain(VkExtent2D Extent, VkFormat Format);
    virtual void Create(VkSwapchainKHR OldSwapChain = VK_NULL_HANDLE);
    void CreateImageViews();

 public:
    static VkFormat swapChainFormat;

    VkSwapchainKHR vkSwapChain;

    SwapChainSupportDetails swapChainSupport;

    std::vector<VkImage> vkImages;
    std::vector<VkImageView> vkImageViews;

    std::unique_ptr<RenderTarget> renderTarget;
    std::unique_ptr<DepthBuffer> depthBuffer;

 protected:
    VkFormat imageFormat;
    VkColorSpaceKHR imageColorSpace;
    VkFormat depthFormat;
    VkExtent2D resolution;
};
}  // namespace Slipper