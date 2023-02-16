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

    // ONLY CALL BEFORE DEVICE IS DESTROYED
    void DestroyDeviceDependencies();

    void RecreateSwapChain(int Width, int Height);
    void RegisterRenderPass(RenderPass &RenderPass);

 private:
    void CleanupSwapChain(bool DestroySwapChain);

 public:
    VkSurfaceKHR vkSurface;
    const Window &window;

    std::unique_ptr<SurfaceSwapChain> swapChain;
    std::vector<RenderPass *> renderPasses;
};
}  // namespace Slipper