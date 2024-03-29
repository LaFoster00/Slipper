#include "../vk_Buffer.h"

namespace Slipper::GPU::Vulkan
{
    Buffer::Buffer(const VkDeviceSize Size, const VkBufferUsageFlags Usage, const VkMemoryPropertyFlags Properties)
        : vkBufferSize(Size)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = vkBufferSize;
        buffer_info.usage = Usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_ASSERT(vkCreateBuffer(device, &buffer_info, nullptr, &vkBuffer), "Failed to create vertex Buffer!")

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(device, vkBuffer, &mem_requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = mem_requirements.size;
        allocInfo.memoryTypeIndex = device.FindMemoryType(mem_requirements.memoryTypeBits,
                                                          vk::MemoryPropertyFlags(Properties));

        VK_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &vkBufferMemory),
                  "Failed to allocate vertex Buffer memory!")
        vkBindBufferMemory(device, vkBuffer, vkBufferMemory, 0);
    }

    Buffer::~Buffer() noexcept
    {
        if (vkBuffer)  // Check if object was moved
        {
            vkDestroyBuffer(device, vkBuffer, nullptr);
            vkFreeMemory(device, vkBufferMemory, nullptr);
        }
    }

    Buffer::Buffer(Buffer &&Source) noexcept
    {
        LOG("Buffer move constructor called.")

        device = Source.device;

        vkBuffer = Source.vkBuffer;
        Source.vkBuffer = VK_NULL_HANDLE;

        vkBufferMemory = Source.vkBufferMemory;
        Source.vkBufferMemory = VK_NULL_HANDLE;

        vkBufferSize = Source.vkBufferSize;
    }

    void Buffer::CopyBuffer(const Buffer &SrcBuffer, const Buffer &DstBuffer)
    {
        SingleUseCommandBuffer command_buffer(*GraphicsEngine::Get().memoryCommandPool);

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = SrcBuffer.vkBufferSize;
        vkCmdCopyBuffer(command_buffer, SrcBuffer, DstBuffer, 1, &copy_region);
        command_buffer.Submit();
    }


    void Buffer::SetBufferData(const ShaderUniformObject *DataObject, const Buffer &Buffer)
    {
        void *data;
        vkMapMemory(VKDevice::Get(), Buffer, 0, Buffer.vkBufferSize, 0, &data);
        memcpy(data, DataObject->GetData(), Buffer.vkBufferSize);
        vkUnmapMemory(VKDevice::Get(), Buffer);
    }
}  // namespace Slipper::GPU::Vulkan
