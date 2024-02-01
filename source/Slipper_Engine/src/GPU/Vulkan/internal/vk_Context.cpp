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
}  // namespace Slipper::GPU::Vulkan