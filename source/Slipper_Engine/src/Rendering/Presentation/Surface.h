#pragma once

#include <memory>

#include "common_includes.h"
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
    Surface(Window &window);
    ~Surface();

    operator VkSurfaceKHR()
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
    void RegisterRenderPass(RenderPass &renderPass);

private:
    void CleanupSwapChain(bool destroySwapChain);

 private:
 public:
    Device *device;

    VkSurfaceKHR surface;
    const Window &window;

    std::unique_ptr<SwapChain> swapChain;
    std::vector<RenderPass *> renderPasses;
};