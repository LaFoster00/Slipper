#pragma once
#include <memory>

#include "Buffer.h"

namespace Slipper
{
class UniformBuffer : public Buffer
{
 public:
    explicit UniformBuffer(VkDeviceSize SizeUniformObject);
    UniformBuffer(UniformBuffer &&Other) noexcept
	    : Buffer(std::move(Other)), m_descriptorInfo(std::move(Other.m_descriptorInfo))
    {
    }

    void SubmitData(const ShaderUniform *UniformData)
    {
        Buffer::SetBufferData(UniformData, *this);
    }

    [[nodiscard]] VkDescriptorBufferInfo *GetDescriptorInfo() const
    {
        return m_descriptorInfo.get();
    }

private:
    std::unique_ptr<VkDescriptorBufferInfo> m_descriptorInfo;
};
}