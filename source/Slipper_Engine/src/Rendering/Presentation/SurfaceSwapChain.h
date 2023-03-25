#pragma once
#include "SwapChain.h"

namespace Slipper
{
class SurfaceSwapChain : public SwapChain
{
 public:
    SurfaceSwapChain(Surface &Surface);
    ~SurfaceSwapChain() override;

    uint32_t GetCurrentSwapChainImageIndex() const override;
    VkResult AcquireNextImageKhr();

 protected:
    void Create(VkSwapchainKHR OldSwapChain) override;
    VkExtent2D ChoseExtent(const Surface &Surface) const;
    VkPresentModeKHR ChoosePresentMode() const;
    VkSurfaceFormatKHR ChooseSurfaceFormat() const;

public:
    Surface &surface;
    SwapChainSupportDetails swapChainSupport;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;

 protected:
    VkColorSpaceKHR imageColorSpace;
    uint32_t m_currentImageIndex = 0;
};
}  // namespace Slipper