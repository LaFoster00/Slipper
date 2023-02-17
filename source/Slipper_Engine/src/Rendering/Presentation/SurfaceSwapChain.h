#pragma once
#include "SwapChain.h"

namespace Slipper
{
class SurfaceSwapChain : public SwapChain
{
 public:
    SurfaceSwapChain(Surface &Surface);
    ~SurfaceSwapChain() override;

 protected:
    void Create(VkSwapchainKHR OldSwapChain) override;
    VkExtent2D ChoseExtent(const Surface &Surface) const;
    VkPresentModeKHR ChoosePresentMode() const;
    VkSurfaceFormatKHR ChooseSurfaceFormat() const;

public:
    Surface &surface;
    SwapChainSupportDetails swapChainSupport;

 protected:
    VkColorSpaceKHR imageColorSpace;
};
}  // namespace Slipper