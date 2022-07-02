#include "Buffer.h"

#include "Drawing/CommandPool.h"
#include "Setup/Device.h"
#include <array>

#include "common_defines.h"

Buffer::Buffer(Device &Device,
               const VkDeviceSize Size,
               const VkBufferUsageFlags Usage,
               const VkMemoryPropertyFlags Properties)
    : vkBufferSize(Size), device(Device)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = vkBufferSize;
    buffer_info.usage = Usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateBuffer(Device, &buffer_info, nullptr, &vkBuffer),
              "Failed to create vertex Buffer!")

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(Device, vkBuffer, &mem_requirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = mem_requirements.size;
    allocInfo.memoryTypeIndex = Device.FindMemoryType(mem_requirements.memoryTypeBits, Properties);

    VK_ASSERT(vkAllocateMemory(Device, &allocInfo, nullptr, &vkBufferMemory),
              "Failed to allocate vertex Buffer memory!")
    vkBindBufferMemory(Device, vkBuffer, vkBufferMemory, 0);
}

Buffer::~Buffer() noexcept
{
    if (vkBuffer)  // Check if object was moved
    {
        vkDestroyBuffer(device, vkBuffer, nullptr);
        vkFreeMemory(device, vkBufferMemory, nullptr);
    }
}

Buffer::Buffer(Buffer &&Source) noexcept : device(Source.device)
{
    LOG("Buffer move constructor called.")

    vkBuffer = Source.vkBuffer;
    Source.vkBuffer = VK_NULL_HANDLE;

    vkBufferMemory = Source.vkBufferMemory;
    Source.vkBufferMemory = VK_NULL_HANDLE;

    vkBufferSize = Source.vkBufferSize;
}

void Buffer::CopyBuffer(const Device &Device,
                        CommandPool &MemoryCommandPool,
                        const Buffer &SrcBuffer,
                        const Buffer &DstBuffer)
{
    const VkCommandBuffer command_buffer = MemoryCommandPool.CreateCommandBuffers(1)[0];

    MemoryCommandPool.BeginCommandBuffer(
        command_buffer, true, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = SrcBuffer.vkBufferSize;
    vkCmdCopyBuffer(command_buffer, SrcBuffer, DstBuffer, 1, &copy_region);

    MemoryCommandPool.EndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(Device.graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(Device.graphicsQueue);

    MemoryCommandPool.DestroyCommandBuffers<1>({command_buffer});
}
