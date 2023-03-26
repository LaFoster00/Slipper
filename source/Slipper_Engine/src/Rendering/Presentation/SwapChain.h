#pragma once

namespace Slipper
{
class RenderPass;
class Framebuffer;
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
        return Impl_GetSwapChain();
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

    void Recreate(uint32_t Width, uint32_t Height);
    void CreateFramebuffers(NonOwningPtr<RenderPass> RenderPass);
    void DestroyFramebuffers(NonOwningPtr<RenderPass> RenderPass);

    virtual VkImage GetCurrentSwapChainImage() const;
    virtual uint32_t GetCurrentSwapChainImageIndex() const = 0;

    std::vector<VkImageView> &GetVkImageViews()
    {
        return m_vkImageViews;
    }

    const std::unordered_map<NonOwningPtr<RenderPass>, std::vector<VkFramebuffer>>
        &GetVkFramebuffers() const
    {
        return m_vkFramebuffers;
    }

	VkFramebuffer GetVkFramebuffer(NonOwningPtr<RenderPass> RenderPass, uint32_t Frame)
    {
        return m_vkFramebuffers[RenderPass][Frame];
    }

 protected:
    SwapChain(VkExtent2D Extent, VkFormat RenderingFormat);

    void Create();
    virtual void Impl_Create() = 0;
    void Cleanup(bool KeepRenderPasses, bool CalledFromDestructor = false);
    virtual void Impl_Cleanup() = 0;

    virtual VkSwapchainKHR Impl_GetSwapChain() const = 0;

    void CreateImageViews();

    std::vector<VkImage> &GetVkImages()
    {
        return m_vkImages;
    }

 public:
    static VkFormat swapChainFormat;
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
    std::unordered_map<NonOwningPtr<RenderPass>, std::vector<VkFramebuffer>> m_vkFramebuffers;
};
}  // namespace Slipper