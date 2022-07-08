#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(const VkDeviceSize SizeUniformObject)
	: Buffer(SizeUniformObject,
	         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{
    m_descriptorInfo = new VkDescriptorBufferInfo();
    m_descriptorInfo->buffer = vkBuffer;
    m_descriptorInfo->offset = 0;
    m_descriptorInfo->range = vkBufferSize;
}

UniformBuffer::~UniformBuffer()
{
    delete m_descriptorInfo;
}
