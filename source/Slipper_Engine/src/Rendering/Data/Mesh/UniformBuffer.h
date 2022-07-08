#pragma once
#include "Buffer.h"

class UniformBuffer : public Buffer
{
 public:
    explicit UniformBuffer(VkDeviceSize SizeUniformObject);
    ~UniformBuffer() override;

    void SubmitData(const ShaderUniform *UniformData)
    {
        Buffer::SetBufferData(UniformData, *this);
    }

    [[nodiscard]] VkDescriptorBufferInfo *GetDescriptorInfo() const
    {
        return m_descriptorInfo;
    }

private:
    VkDescriptorBufferInfo *m_descriptorInfo{};
};
