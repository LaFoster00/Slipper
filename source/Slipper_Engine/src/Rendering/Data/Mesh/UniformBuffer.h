#pragma once
#include "Buffer.h"

class UniformBuffer : public Buffer
{
public:
	explicit UniformBuffer(VkDeviceSize SizeUniformObject);

    void SubmitData(const ShaderUniform *UniformData) const
    {
        SetBufferData(UniformData, *this);
    }
};
