#include "CommandPool.h"

#include "GraphicsEngine.h"

namespace Slipper
{
CommandPool::CommandPool(const vk::Queue Queue,
                         const uint32_t QueueFamilyIndex,
                         const int32_t BufferCount)
    : m_queue(Queue)
{
	const vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, QueueFamilyIndex);
    VK_HPP_ASSERT(device.logicalDevice.createCommandPool(&pool_info, nullptr, &m_vkCommandPool), "Failed to create command pool");
    CreateCommandBuffers(BufferCount);
}

CommandPool::~CommandPool()
{
    device.logicalDevice.destroyCommandPool(m_vkCommandPool);
}

std::vector<vk::CommandBuffer> &CommandPool::CreateCommandBuffers(
    const int32_t BufferCount, int32_t *NewCommandBufferStartIndex)
{
    if (BufferCount > 0) {
        const auto start_index = static_cast<int32_t>(m_vkCommandBuffers.size());
        if (NewCommandBufferStartIndex) {
            *NewCommandBufferStartIndex = start_index;
        }
        m_vkCommandBuffers.resize(m_vkCommandBuffers.size() + BufferCount);


        const vk::CommandBufferAllocateInfo alloc_info(
            m_vkCommandPool, vk::CommandBufferLevel::ePrimary, BufferCount);
        // Get pointer into the array at the location of the new command buffers
        const auto offset_data_pointer = &m_vkCommandBuffers[start_index];

        VK_HPP_ASSERT(
            device.logicalDevice.allocateCommandBuffers(&alloc_info, offset_data_pointer), "Failed to create command buffers");
    }
    return m_vkCommandBuffers;
}

void CommandPool::DestroyCommandBuffers(const std::vector<vk::CommandBuffer> &CommandBuffers)
{
    device.logicalDevice.freeCommandBuffers(m_vkCommandPool, CommandBuffers);

    for (auto command_buffer : CommandBuffers) {
        if (auto buffer_loc = std::ranges::find(m_vkCommandBuffers, command_buffer);
            buffer_loc != m_vkCommandBuffers.end()) {
            m_vkCommandBuffers.erase(buffer_loc);
        }
    }
}

void CommandPool::BeginCommandBuffer(const vk::CommandBuffer CommandBuffer,
                                     const bool ResetCommandBuffer,
                                     const bool SingleUseBuffer,
                                     const vk::CommandBufferUsageFlags Flags)
{
    vk::CommandBuffer buffer = nullptr;
    if (SingleUseBuffer && std::ranges::find(m_singleUseVkCommandBuffers, CommandBuffer) !=
                               m_singleUseVkCommandBuffers.end()) {
        buffer = CommandBuffer;
    }
    else if (std::ranges::find(m_vkCommandBuffers, CommandBuffer) != m_vkCommandBuffers.end()) {
        buffer = CommandBuffer;
    }

    if (buffer) {

        if (ResetCommandBuffer) {
            buffer.reset();
        }

        const vk::CommandBufferBeginInfo begin_info(Flags);
        VK_HPP_ASSERT(buffer.begin(&begin_info), "Failed to begin recording command buffer!");

        return;
    }

    ASSERT(1, "The command buffer is not part of the command pool.");
}

void CommandPool::EndCommandBuffer(const vk::CommandBuffer CommandBuffer) const
{
    VK_ASSERT(vkEndCommandBuffer(CommandBuffer), "Failed to record to command buffer!");
}

void CommandPool::ClearSingleUseCommands()
{
    if (!m_singleUseVkCommandBuffers.empty()) {
        device.logicalDevice.freeCommandBuffers(m_vkCommandPool, m_singleUseVkCommandBuffers);
        m_singleUseVkCommandBuffers.clear();
    }
}

vk::CommandBuffer CommandPool::GetCurrentCommandBuffer() const
{
	return m_vkCommandBuffers[GraphicsEngine::Get().GetCurrentFrame()];
}

vk::CommandBuffer CommandPool::CreateSingleUseCommandBuffer()
{
    const auto start_index = static_cast<int32_t>(m_singleUseVkCommandBuffers.size());
    m_singleUseVkCommandBuffers.resize(m_singleUseVkCommandBuffers.size() + 1);

    const vk::CommandBufferAllocateInfo alloc_info(
        m_vkCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    const auto offset_data_pointer = &m_singleUseVkCommandBuffers[start_index];
    VK_HPP_ASSERT(device.logicalDevice.allocateCommandBuffers(&alloc_info, offset_data_pointer),
                  "Failed to create single use command buffer")

    return m_singleUseVkCommandBuffers.back();
}

void CommandPool::DestroySingleUseCommandBuffer(const vk::CommandBuffer CommandBuffer)
{
    VKDevice::GetVk().freeCommandBuffers(m_vkCommandPool, CommandBuffer);

    if (const auto buffer_loc = std::ranges::find(m_singleUseVkCommandBuffers, CommandBuffer);
        buffer_loc != m_singleUseVkCommandBuffers.end()) {
        m_singleUseVkCommandBuffers.erase(buffer_loc);
    }
}
}  // namespace Slipper