#pragma once

#include "DeviceDependentObject.h"
#include "GraphicsPipeline/Shader.h"
#include "Setup/Device.h"
#include "common_includes.h"

struct ShaderUniform;
class CommandPool;

class Buffer : DeviceDependentObject
{
 public:
    Buffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);

    virtual ~Buffer() noexcept;

    Buffer(Buffer &&Source) noexcept;

    /* Sets the buffers data to that of the supplied pointer based on the size specified during creation. */
    template<typename TDataObject>
	static void SetBufferData(const TDataObject DataObject,
                              const Buffer &Buffer)
	requires std::is_pointer_v<TDataObject>
    {
        void *data;
        vkMapMemory(Device::Get(), Buffer, 0, Buffer.vkBufferSize, 0, &data);
        memcpy(data, DataObject, static_cast<size_t>(Buffer.vkBufferSize));
        vkUnmapMemory(Device::Get(), Buffer);
    }

    static void CopyBuffer(const Buffer &SrcBuffer,
                           const Buffer &DstBuffer);

    operator VkBuffer() const
    {
        return vkBuffer;
    }

    operator VkDeviceMemory() const
    {
        return vkBufferMemory;
    }

 public:
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize vkBufferSize;
};

template<> void Buffer::SetBufferData(const ShaderUniform *DataObject, const Buffer &Buffer);