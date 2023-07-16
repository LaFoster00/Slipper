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

    [[nodiscard]] const vk::Extent2D &GetResolution() const
    {
        return resolution;
    }

    [[nodiscard]] const vk::Format &GetImageFormat() const
    {
        return imageRenderingFormat;
    }

    [[nodiscard]] const vk::Format &GetDepthFormat() const
    {
        return depthFormat;
    }

    void Recreate(uint32_t Width, uint32_t Height);
    void CreateFramebuffers(NonOwningPtr<RenderPass> RenderPass);
    void DestroyFramebuffers(NonOwningPtr<RenderPass> RenderPass);

    virtual vk::Image GetCurrentSwapChainImage() const;
    virtual uint32_t GetCurrentSwapChainImageIndex() const = 0;

    std::vector<vk::ImageView> &GetVkImageViews()
    {
        return m_vkImageViews;
    }

    const std::unordered_map<NonOwningPtr<RenderPass>, std::vector<vk::Framebuffer>>
        &GetVkFramebuffers() const
    {
        return m_vkFramebuffers;
    }

    VkFramebuffer GetVkFramebuffer(NonOwningPtr<RenderPass> RenderPass, uint32_t Frame)
    {
        return m_vkFramebuffers[RenderPass][Frame];
    }

 protected:
    SwapChain(vk::Extent2D Extent, vk::Format RenderingFormat);

    void Create();
    virtual void Impl_Create() = 0;
    void Cleanup(bool KeepRenderPasses, bool CalledFromDestructor = false);
    virtual void Impl_Cleanup() = 0;

    virtual VkSwapchainKHR Impl_GetSwapChain() const = 0;

    void CreateImageViews();

    std::vector<vk::Image> &GetVkImages()
    {
        return m_vkImages;
    }

 public:
    static inline vk::Format swapChainFormat = Engine::TARGET_WINDOW_COLOR_FORMAT;
    SwapChainSupportDetails swapChainSupport;

    std::unique_ptr<RenderTarget> renderTarget;
    std::unique_ptr<DepthBuffer> depthBuffer;

 protected:
    vk::Format imageRenderingFormat;
    vk::ColorSpaceKHR imageColorSpace;
    vk::Format depthFormat;
    vk::Extent2D resolution;

 private:
    std::vector<vk::Image> m_vkImages;
    std::vector<vk::ImageView> m_vkImageViews;
    std::unordered_map<NonOwningPtr<RenderPass>, std::vector<vk::Framebuffer>> m_vkFramebuffers;
};
}  // namespace Slipper