#include "CommandPool.h"

#include "GraphicsPipeline/RenderPass.h"
#include "Setup/Device.h"
#include "common_defines.h"

#include <algorithm>

namespace Slipper
{
CommandPool::CommandPool(const VkQueue Queue,
                         const uint32_t QueueFamilyIndex,
                         const int32_t BufferCount)
    : m_queue(Queue)
{
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = QueueFamilyIndex;

    VK_ASSERT(vkCreateCommandPool(device.logicalDevice, &pool_info, nullptr, &vkCommandPool),
              "Failed to create command pool");

    CreateCommandBuffers(BufferCount);
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device.logicalDevice, vkCommandPool, nullptr);
}

std::vector<VkCommandBuffer> &CommandPool::CreateCommandBuffers(
    const int32_t BufferCount, int32_t *NewCommandBufferStartIndex)
{
    if (BufferCount > 0) {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = vkCommandPool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = BufferCount;

        const auto start_index = static_cast<int32_t>(vkCommandBuffers.size());
        if (NewCommandBufferStartIndex) {
            *NewCommandBufferStartIndex = start_index;
        }
        vkCommandBuffers.resize(vkCommandBuffers.size() + BufferCount);

        // Get pointer into the array at the location of the new command buffers
        const auto offset_data_pointer = &vkCommandBuffers[start_index];

        VK_ASSERT(vkAllocateCommandBuffers(device.logicalDevice, &alloc_info, offset_data_pointer),
                  "Failed to create command buffers!")
    }
    return vkCommandBuffers;
}

void CommandPool::DestroyCommandBuffers(const std::vector<VkCommandBuffer> &CommandBuffers)
{
    vkFreeCommandBuffers(
        device, vkCommandPool, static_cast<int32_t>(CommandBuffers.size()), CommandBuffers.data());

    for (auto command_buffer : CommandBuffers) {
        if (auto buffer_loc = std::ranges::find(vkCommandBuffers, command_buffer);
            buffer_loc != vkCommandBuffers.end()) {
            vkCommandBuffers.erase(buffer_loc);
        }
    }
}

void CommandPool::BeginCommandBuffer(const VkCommandBuffer CommandBuffer,
                                     const bool ResetCommandBuffer,
                                     const bool SingleUseBuffer,
                                     const VkCommandBufferUsageFlags Flags)
{
    VkCommandBuffer buffer = nullptr;
    if (SingleUseBuffer && std::ranges::find(m_singleUseVkCommandBuffers, CommandBuffer) !=
                               m_singleUseVkCommandBuffers.end()) {
        buffer = CommandBuffer;
    }
    else if (std::ranges::find(vkCommandBuffers, CommandBuffer) != vkCommandBuffers.end()) {
        buffer = CommandBuffer;
    }

    if (buffer) {

        if (ResetCommandBuffer) {
            vkResetCommandBuffer(buffer, 0);
        }

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = Flags;               // Optional
        begin_info.pInheritanceInfo = nullptr;  // Optional

        VK_ASSERT(vkBeginCommandBuffer(buffer, &begin_info),
                  "Failed to begin recording command buffer!");

        return;
    }

    ASSERT(1, "The command buffer is not part of the command pool.");
}

void CommandPool::EndCommandBuffer(const VkCommandBuffer CommandBuffer) const
{
    VK_ASSERT(vkEndCommandBuffer(CommandBuffer), "Failed to record to command buffer!");
}

void CommandPool::ClearSingleUseCommands()
{
    if (!m_singleUseVkCommandBuffers.empty()) {
        vkFreeCommandBuffers(device,
                             vkCommandPool,
                             static_cast<int32_t>(m_singleUseVkCommandBuffers.size()),
                             m_singleUseVkCommandBuffers.data());
        m_singleUseVkCommandBuffers.clear();
    }
}

VkCommandBuffer CommandPool::CreateSingleUseCommandBuffer()
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = vkCommandPool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    const auto start_index = static_cast<int32_t>(m_singleUseVkCommandBuffers.size());

    m_singleUseVkCommandBuffers.resize(m_singleUseVkCommandBuffers.size() + 1);
    const auto offset_data_pointer = &m_singleUseVkCommandBuffers.data()[start_index];

    VK_ASSERT(vkAllocateCommandBuffers(device.logicalDevice, &alloc_info, offset_data_pointer),
              "Failed to create single use command buffer!")

    return m_singleUseVkCommandBuffers.back();
}

void CommandPool::DestroySingleUseCommandBuffer(const VkCommandBuffer CommandBuffer)
{
    vkFreeCommandBuffers(device, vkCommandPool, 1, &CommandBuffer);

    if (const auto buffer_loc = std::ranges::find(m_singleUseVkCommandBuffers, CommandBuffer);
        buffer_loc != m_singleUseVkCommandBuffers.end()) {
        m_singleUseVkCommandBuffers.erase(buffer_loc);
    }
}
}  // namespace Slipper