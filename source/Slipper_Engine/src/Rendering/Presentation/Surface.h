#pragma once

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

    operator vk::SurfaceKHR&()
    {
        return vkSurface;
    }

    operator const vk::SurfaceKHR &() const
    {
        return vkSurface;
    }

    [[nodiscard]] const VkExtent2D &GetResolution() const
    {
        return swapChain->GetResolution();
    }

    // ONLY CALL AFTER DEVICE IS CREATED
    void CreateSwapChain();

 private:
    void CleanupSwapChain(bool DestroySwapChain);

 public:
    vk::SurfaceKHR vkSurface;
    const Window &window;

    OwningPtr<SurfaceSwapChain> swapChain;
};
}  // namespace Slipper