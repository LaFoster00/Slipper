#pragma once
#include "Buffer.h"

class UniformBuffer : public Buffer
{
public:
    UniformBuffer(VkDeviceSize SizeUniformObject);

    void SubmitData(const ShaderUniform *uniformData) const
    {
        SetBufferData(uniformData, *this);
    }
};
