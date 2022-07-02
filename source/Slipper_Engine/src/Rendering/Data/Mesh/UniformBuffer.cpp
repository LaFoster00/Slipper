#include "UniformBuffer.h"

#include "Setup/Device.h"

UniformBuffer::UniformBuffer(const VkDeviceSize SizeUniformObject)
	: Buffer(Device::Get(),
	         SizeUniformObject,
	         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
}
