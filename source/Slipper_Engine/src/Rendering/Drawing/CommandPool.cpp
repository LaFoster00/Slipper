#include "CommandPool.h"

#include "../GraphicsPipeline/RenderPass.h"
#include "../Setup/Device.h"

CommandPool::CommandPool(Device &device, int32_t BufferCount) : device(device)
{
    QueueFamilyIndices &queueFamilyIndices = device.queueFamilyIndices;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VK_ASSERT(vkCreateCommandPool(device.logicalDevice, &poolInfo, nullptr, &vkCommandPool),
              "Failed to create command pool");

    CreateCommandBuffers(BufferCount);
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device.logicalDevice, vkCommandPool, nullptr);
}

std::vector<VkCommandBuffer> &CommandPool::CreateCommandBuffers(int32_t BufferCount)
{
    if (BufferCount > 0) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vkCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = BufferCount;

        vkCommandBuffers.resize(vkCommandBuffers.size() + BufferCount);

        VK_ASSERT(
            vkAllocateCommandBuffers(device.logicalDevice, &allocInfo, vkCommandBuffers.data()),
            "Failed to create command buffers!")
    }
    return vkCommandBuffers;
}

VkCommandBuffer CommandPool::BeginCommandBuffer(uint32_t bufferIndex,
                                                bool resetCommandBuffer,
                                                VkCommandBufferUsageFlags flags)
{
    if (resetCommandBuffer)
        vkResetCommandBuffer(vkCommandBuffers[bufferIndex], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                   // Optional
    beginInfo.pInheritanceInfo = nullptr;  // Optional

    VK_ASSERT(vkBeginCommandBuffer(vkCommandBuffers[bufferIndex], &beginInfo),
              "Failed to begin recording command buffer!");

    return vkCommandBuffers[bufferIndex];
}

VkCommandBuffer CommandPool::BeginCommandBuffer(VkCommandBuffer commandBuffer,
                                                bool resetCommandBuffer,
                                                VkCommandBufferUsageFlags flags)
{
    const auto commandBufferLocation = std::find(
        vkCommandBuffers.begin(), vkCommandBuffers.end(), commandBuffer);
    if (commandBufferLocation != vkCommandBuffers.end()) {
        return BeginCommandBuffer(
            static_cast<uint32_t>(commandBufferLocation - vkCommandBuffers.begin()),
            resetCommandBuffer,
            flags);
    }
    else {
        ASSERT(1, "The command buffer is not part of the command pool.");
    }
}

void CommandPool::EndCommandBuffer(VkCommandBuffer commandBuffer)
{
    VK_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to record to command buffer!");
}

void CommandPool::EndCommandBuffer(uint32_t bufferIndex)
{
    EndCommandBuffer(vkCommandBuffers[bufferIndex]);
}