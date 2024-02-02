#pragma once
#include "Context.h"

namespace Slipper::GPU::Vulkan
{
class VKDevice;
class VKContext : public Context
{
 public:
    explicit VKContext(VKDevice &device) : m_device(device)
    {
    }

    void WaitIdle() override;
    Engine::GpuBackend BackendType() override;
    void CreateSwapChain() override;
    NonOwningPtr<Vulkan::SwapChain> GetSwapchain() override;
    
    glm::int2 GetResolution() override;

private:
    vk::SurfaceKHR GetSurface();

private:
    VKDevice &m_device;
};
}  // namespace Slipper::GPU::Vulkan