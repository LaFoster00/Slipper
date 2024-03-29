#pragma once
#include "vk_SwapChain.h"

namespace Slipper::GPU::Vulkan
{
    class SurfaceSwapChain : public SwapChain
    {
     public:
        SurfaceSwapChain(Surface &Surface);
        ~SurfaceSwapChain() override;

        uint32_t GetCurrentSwapChainImageIndex() const override;
        VkResult AcquireNextImageKhr();

     protected:
        void Impl_Create() override;
        VkExtent2D ChoseExtent(const Surface &Surface) const;
        VkPresentModeKHR ChoosePresentMode() const;
        vk::SurfaceFormatKHR ChooseSurfaceFormat() const;
        void Impl_Cleanup() override;
        VkSwapchainKHR Impl_GetSwapChain() const override;

     public:
        Surface &surface;
        SwapChainSupportDetails swapChainSupport;
        VkSwapchainKHR vkSwapChain;

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

     protected:
        vk::ColorSpaceKHR imageColorSpace;
        uint32_t m_currentImageIndex = 0;
    };
}  // namespace Slipper::GPU::Vulkan
