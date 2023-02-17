#pragma once

#include <memory>

#include "SurfaceSwapChain.h"

namespace Slipper
{
class RenderPass;

class Surface
{
 public:
    Surface(const Window &Window);
    ~Surface();

    operator VkSurfaceKHR() const
    {
        return vkSurface;
    }

    [[nodiscard]] const VkExtent2D &GetResolution() const
    {
        return swapChain->GetResolution();
    }

    // ONLY CALL AFTER DEVICE IS CREATED
    void CreateSwapChain();

    void RecreateSwapChain(uint32_t Width, uint32_t Height);
    void RegisterRenderPass(RenderPass &RenderPass);
    void UnregisterRenderPass(RenderPass &RenderPass);

 private:
    void CleanupSwapChain(bool DestroySwapChain);

 public:
    VkSurfaceKHR vkSurface;
    const Window &window;

    std::unique_ptr<SurfaceSwapChain> swapChain;
    std::vector<RenderPass *> renderPasses;
};
}  // namespace Slipper