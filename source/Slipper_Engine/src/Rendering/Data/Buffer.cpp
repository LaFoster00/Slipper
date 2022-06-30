#include "Buffer.h"

#include "Drawing/CommandPool.h"
#include "Setup/Device.h"
#include <array>

Buffer::Buffer(Device &Device,
               VkDeviceSize Size,
               VkBufferUsageFlags Usage,
               VkMemoryPropertyFlags Properties)
    : vkBufferSize(Size), device(Device)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vkBufferSize;
    bufferInfo.usage = Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &vkBuffer),
              "Failed to create vertex Buffer!")

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, Properties);

    VK_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &vkBufferMemory),
              "Failed to allocate vertex Buffer memory!")
    vkBindBufferMemory(device, vkBuffer, vkBufferMemory, 0);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(device, vkBuffer, nullptr);
    vkFreeMemory(device, vkBufferMemory, nullptr);
}

void Buffer::CopyBuffer(Device &device, CommandPool& MemoryCommandPool, const Buffer& srcBuffer, const Buffer& dstBuffer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = MemoryCommandPool;
    allocInfo.commandBufferCount = 1;

    const VkCommandBuffer commandBuffer = MemoryCommandPool.CreateCommandBuffers(1)[0];

    MemoryCommandPool.BeginCommandBuffer(
        commandBuffer, true, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = srcBuffer.vkBufferSize;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    MemoryCommandPool.EndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.graphicsQueue);

    MemoryCommandPool.DestroyCommandBuffers<1>({commandBuffer});
}

