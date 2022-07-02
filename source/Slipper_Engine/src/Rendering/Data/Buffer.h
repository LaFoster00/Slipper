#pragma once

#include "common_includes.h"
#include "GraphicsPipeline/Shader.h"
#include "Setup/Device.h"

class CommandPool;
class Device;

class Buffer
{
 public:
    Buffer(Device &Device,
           VkDeviceSize Size,
           VkBufferUsageFlags Usage,
           VkMemoryPropertyFlags Properties);

    virtual ~Buffer() noexcept;

    Buffer(Buffer &&Source) noexcept;

    template<typename TDataObject>
    static void SetBufferData(const TDataObject *DataObject, const Buffer &Buffer)
    {
        void *data;
        vkMapMemory(Device::Get(), Buffer, 0, Buffer.vkBufferSize, 0, &data);
        memcpy(data, DataObject, static_cast<size_t>(Buffer.vkBufferSize));
        vkUnmapMemory(Device::Get(), Buffer);
    }

    template<>
    static void SetBufferData(const ShaderUniform *DataObject, const Buffer &Buffer)
    {
        void *data;
        vkMapMemory(Device::Get(), Buffer, 0, Buffer.vkBufferSize, 0, &data);
        memcpy(data, DataObject->GetData(), Buffer.vkBufferSize);
        vkUnmapMemory(Device::Get(), Buffer);
    }

    static void CopyBuffer(const Device &Device,
                           CommandPool &MemoryCommandPool,
                           const Buffer &SrcBuffer,
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

    Device &device;
};
