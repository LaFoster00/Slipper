#pragma once
#include "SwapChain.h"

namespace Slipper
{
class OffscreenSwapChain : public SwapChain
{
 public:
    OffscreenSwapChain(const VkExtent2D &Extent,
                       VkFormat RenderingFormat,
                       uint32_t NumImages,
                       bool WithPresentationTextures);

    ~OffscreenSwapChain() override;
    uint32_t GetCurrentSwapChainImageIndex() const override;

    void UpdatePresentationTextures(VkCommandBuffer CommandBuffer, uint32_t ImageIndex) const;

protected:
    void Impl_Create() override;
    void Impl_Cleanup() override;
    VkSwapchainKHR Impl_GetSwapChain() const override;

public:
    

    bool withPresentationTextures;
    uint32_t numImages;
    std::vector<OwningPtr<Texture2D>> presentationTextures;

 protected:
    std::vector<VkDeviceMemory> vkImageMemory;
};
}  // namespace Slipper