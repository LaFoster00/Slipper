#pragma once

#include <memory>

#include "DeviceDependentObject.h"
#include "SwapChain.h"

class RenderPass;
class SwapChain;
class Instance;
class Device;
class Window;

class Surface
{
 public:
	explicit Surface(const Window &Window);
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

    void RecreateSwapChain();
    void RegisterRenderPass(RenderPass &RenderPass);

 private:
    void CleanupSwapChain(bool DestroySwapChain);
    
 public:
    Device *device;

    VkSurfaceKHR surface;
    const Window &window;

    std::unique_ptr<SwapChain> swapChain;
    std::vector<RenderPass *> renderPasses;
};
