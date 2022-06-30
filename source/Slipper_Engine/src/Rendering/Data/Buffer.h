#pragma once

#include "common_includes.h"

class CommandPool;
class Device;

class Buffer
{
 public:
    Buffer(Device &Device,
           VkDeviceSize Size,
           VkBufferUsageFlags Usage,
           VkMemoryPropertyFlags Properties);

    ~Buffer();

    static void CopyBuffer(Device &device, CommandPool &MemoryCommandPool, const Buffer &srcBuffer, const Buffer &dstBuffer);

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
