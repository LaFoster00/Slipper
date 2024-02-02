#include "CommandPool.h"

#include "Vulkan/vk_CommandPool.h"
#include "Vulkan/vk_Device.h"

namespace Slipper::GPU
{

    CommandPool * CommandPool::Create()
    {
        if (Engine::GPU_BACKEND == Engine::GpuBackend::Vulkan)
        {
            return new Vulkan::CommandPool(Vulkan::VKDevice::Get().transferQueue,
                                           Vulkan::VKDevice::Get().queueFamilyIndices.transferFamily.value());
        }
    }
}
