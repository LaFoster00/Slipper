#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(const VkDeviceSize SizeUniformObject)
	: Buffer(SizeUniformObject,
	         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
}
