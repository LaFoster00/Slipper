#include "../vk_DeviceDependentObject.h"

#include "Vulkan/vk_Device.h"

namespace Slipper::GPU::Vulkan
{
    // TODO extend for other backends
    DeviceDependentObject::DeviceDependentObject()
        : device(VKDevice::Get())
    {
    }
}  // namespace Slipper::GPU
