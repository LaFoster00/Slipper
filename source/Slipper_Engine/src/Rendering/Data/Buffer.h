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

    Buffer(Buffer &&source) noexcept;

    template<typename DataObject>
    static void SetBufferData(const DataObject *dataObject, const Buffer &buffer)
    {
        void *data;
        vkMapMemory(Device::Get(), buffer, 0, buffer.vkBufferSize, 0, &data);
        memcpy(data, dataObject, (size_t)buffer.vkBufferSize);
        vkUnmapMemory(Device::Get(), buffer);
    }

    template<>
    static void SetBufferData(const ShaderUniform *dataObject, const Buffer &buffer)
    {
        void *data;
        vkMapMemory(Device::Get(), buffer, 0, buffer.vkBufferSize, 0, &data);
        memcpy(data, dataObject->GetData(), (size_t)buffer.vkBufferSize);
        vkUnmapMemory(Device::Get(), buffer);
    }

    static void CopyBuffer(Device &device,
                           CommandPool &MemoryCommandPool,
                           const Buffer &srcBuffer,
                           const Buffer &dstBuffer);

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
