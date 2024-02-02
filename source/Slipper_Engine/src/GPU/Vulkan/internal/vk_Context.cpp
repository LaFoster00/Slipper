#include "../vk_Context.h"

#include "Vulkan/vk_Device.h"

namespace Slipper::GPU::Vulkan
{
    void VKContext::WaitIdle()
    {
        m_device.logicalDevice.waitIdle();
    }

    Engine::GpuBackend VKContext::BackendType()
    {
        return Engine::GpuBackend::Vulkan;
    }

    glm::int2 VKContext::GetResolution()
    {
        const auto capabilities = m_device.physicalDevice.getSurfaceCapabilitiesKHR(GetSurface());
        return glm::int2(capabilities.currentExtent.width, capabilities.currentExtent.height);
    }
}  // namespace Slipper::GPU::Vulkan
