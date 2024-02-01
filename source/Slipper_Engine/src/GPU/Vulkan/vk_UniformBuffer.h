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

    void SubmitData(const ShaderUniformObject *UniformData)
    {
        Buffer::SetBufferData(UniformData, *this);
    }

    [[nodiscard]] VkDescriptorBufferInfo *GetDescriptorInfo() const
    {
        return m_descriptorInfo.get();
    }

    [[nodiscard]] constexpr vk::DescriptorType GetDescriptorType() const override
    {
        return vk::DescriptorType::eUniformBuffer;
    }

    void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const override
    {
        ASSERT(Binding.size == vkBufferSize,
               "Buffer size mismatch: Shader Binding -> {} | Supplied Buffer -> {}",
               Binding.size,
               vkBufferSize);
    }

private:
    std::unique_ptr<VkDescriptorBufferInfo> m_descriptorInfo;
};
}