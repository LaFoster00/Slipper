#pragma once

namespace Slipper::GPU::Vulkan
{
    class VKDevice;

    class DeviceDependentObject
    {
     public:
        DeviceDependentObject();

     public:
        VKDevice &device;
    };
}  // namespace Slipper::GPU::Vulkan
