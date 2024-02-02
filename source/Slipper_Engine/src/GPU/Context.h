#pragma once
#include "Object.h"
#include "Vulkan/vk_SwapChain.h"

namespace Slipper
{
    class Window;
}

namespace Slipper::GPU
{
    class Context : Object<Context>
    {
     public:
        static Context *CreateContext(Window &window);

        virtual void WaitIdle() = 0;
        virtual Engine::GpuBackend BackendType() = 0;
        virtual void CreateSwapChain() = 0;
        virtual NonOwningPtr<Vulkan::SwapChain> GetSwapchain() = 0;
        virtual vk::SurfaceKHR GetSurface() = 0;
        virtual glm::int2 GetResolution() = 0;
    };
}  // namespace Slipper::GPU
