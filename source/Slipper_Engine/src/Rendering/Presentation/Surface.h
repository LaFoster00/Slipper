#pragma once

#include <memory>

#include "SwapChain.h"

namespace Slipper
{
class RenderPass;
class SwapChain;
class VulkanInstance;
class Device;
class Window;

class Surface
{
 public:
    Surface(const Window &Window);
    ~Surface();

    operator VkSurfaceKHR() const
    {
        return surface;
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
    VkSurfaceKHR surface;
    const Window &window;

    std::unique_ptr<SwapChain> swapChain;
    std::vector<RenderPass *> renderPasses;
};
}  // namespace Slipper