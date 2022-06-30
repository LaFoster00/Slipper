#include "UniformBuffer.h"

#include "Setup/Device.h"

UniformBuffer::UniformBuffer(VkDeviceSize SizeUniformBuffer)
    : Buffer(Device::Get(),
             SizeUniformBuffer,
             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
}
