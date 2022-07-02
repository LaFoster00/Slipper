#include "CommandPool.h"

#include "../GraphicsPipeline/RenderPass.h"
#include "../Setup/Device.h"
#include "common_defines.h"

CommandPool::CommandPool(Device &Device, const int32_t BufferCount) : device(Device)
{
    const auto &[graphicsFamily, presentFamily] = Device.queueFamilyIndices;

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = graphicsFamily.value();

    VK_ASSERT(vkCreateCommandPool(Device.logicalDevice, &pool_info, nullptr, &vkCommandPool),
              "Failed to create command pool");

    CreateCommandBuffers(BufferCount);
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device.logicalDevice, vkCommandPool, nullptr);
}

std::vector<VkCommandBuffer> &CommandPool::CreateCommandBuffers(const int32_t BufferCount)
{
    if (BufferCount > 0) {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = vkCommandPool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = BufferCount;

        vkCommandBuffers.resize(vkCommandBuffers.size() + BufferCount);

        VK_ASSERT(
            vkAllocateCommandBuffers(device.logicalDevice, &alloc_info, vkCommandBuffers.data()),
            "Failed to create command buffers!")
    }
    return vkCommandBuffers;
}

VkCommandBuffer CommandPool::BeginCommandBuffer(uint32_t BufferIndex,
                                                bool ResetCommandBuffer,
                                                VkCommandBufferUsageFlags Flags) const
{
    if (ResetCommandBuffer) {
        vkResetCommandBuffer(vkCommandBuffers[BufferIndex], 0);
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;                   // Optional
    begin_info.pInheritanceInfo = nullptr;  // Optional

    VK_ASSERT(vkBeginCommandBuffer(vkCommandBuffers[BufferIndex], &begin_info),
              "Failed to begin recording command buffer!");

    return vkCommandBuffers[BufferIndex];
}

VkCommandBuffer CommandPool::BeginCommandBuffer(const VkCommandBuffer CommandBuffer,
                                                const bool ResetCommandBuffer,
                                                const VkCommandBufferUsageFlags Flags)
{
    if (const auto command_buffer_location = std::ranges::find(vkCommandBuffers, CommandBuffer);
        command_buffer_location != vkCommandBuffers.end()) {
        return BeginCommandBuffer(
            static_cast<uint32_t>(command_buffer_location - vkCommandBuffers.begin()),
            ResetCommandBuffer,
            Flags);
    }

    ASSERT(1, "The command buffer is not part of the command pool.");
}

void CommandPool::EndCommandBuffer(const VkCommandBuffer CommandBuffer) const
{
    VK_ASSERT(vkEndCommandBuffer(CommandBuffer), "Failed to record to command buffer!");
}

void CommandPool::EndCommandBuffer(uint32_t BufferIndex) const
{
    EndCommandBuffer(vkCommandBuffers[BufferIndex]);
}
