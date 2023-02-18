#pragma once
#include "SwapChain.h"

namespace Slipper
{
class OffscreenSwapChain : public SwapChain
{
 public:
    OffscreenSwapChain(const VkExtent2D &Extent,
                       VkFormat RenderingFormat,
                       uint32_t NumImages);

    ~OffscreenSwapChain() override;

    void ClearImages() override;

 protected:
    void Create(VkSwapchainKHR OldSwapChain = VK_NULL_HANDLE) override;

 public:
    uint32_t numImages;

 protected:
    std::vector<VkDeviceMemory> vkImageMemory;
};
}  // namespace Slipper