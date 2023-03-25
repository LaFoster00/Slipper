#pragma once

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
        return imageRenderingFormat;
    }

    [[nodiscard]] const VkFormat &GetDepthFormat() const
    {
        return depthFormat;
    }

	virtual void ClearImages();
    virtual void Recreate(uint32_t Width, uint32_t Height);

    virtual VkImage GetCurrentSwapChainImage() const ;
    virtual uint32_t GetCurrentSwapChainImageIndex() const = 0;

    std::vector<VkImageView> &GetVkImageViews()
    {
        return m_vkImageViews;
    }

 protected:
    SwapChain(VkExtent2D Extent,
              VkFormat RenderingFormat);
    virtual void Create(VkSwapchainKHR OldSwapChain = VK_NULL_HANDLE);
    void CreateImageViews();

    std::vector<VkImage> &GetVkImages()
    {
        return m_vkImages;
    }

 public:
    static VkFormat swapChainFormat;

    VkSwapchainKHR vkSwapChain;
    SwapChainSupportDetails swapChainSupport;

    std::unique_ptr<RenderTarget> renderTarget;
    std::unique_ptr<DepthBuffer> depthBuffer;

 protected:
    VkFormat imageRenderingFormat;
    VkColorSpaceKHR imageColorSpace;
    VkFormat depthFormat;
    VkExtent2D resolution;

private:
    std::vector<VkImage> m_vkImages;
    std::vector<VkImageView> m_vkImageViews;
};
}  // namespace Slipper