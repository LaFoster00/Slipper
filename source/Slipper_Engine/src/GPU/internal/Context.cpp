#include "Context.h"

#include "Vulkan/vk_Context.h"
#include "Vulkan/vk_Device.h"

namespace Slipper::GPU
{
	Context* Context::CreateContext(Window& window)
	{
        if (Engine::GPU_BACKEND == Engine::GpuBackend::Vulkan)
        {
            return new Vulkan::VKContext(Vulkan::VKDevice::Get());
        }
	}
}
