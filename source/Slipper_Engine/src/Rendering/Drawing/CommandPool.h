#pragma once

#include <optional>

#include "DeviceDependentObject.h"
#include "Setup/Device.h"

struct SingleUseCommandBuffer;
class RenderPass;

class CommandPool : DeviceDependentObject
{
    friend SingleUseCommandBuffer;

 public:
    explicit CommandPool(VkQueue Queue, uint32_t QueueFamilyIndex, int32_t BufferCount = 0);
    ~CommandPool();

    std::vector<VkCommandBuffer> &CreateCommandBuffers(
        int32_t BufferCount, int32_t *NewCommandBufferStartIndex = nullptr);
    void DestroyCommandBuffers(const std::vector<VkCommandBuffer> &CommandBuffers);

    void BeginCommandBuffer(VkCommandBuffer CommandBuffer,
                            bool ResetCommandBuffer = true,
                            bool SingleUseBuffer = false,
                            VkCommandBufferUsageFlags Flags = 0);
    void EndCommandBuffer(VkCommandBuffer CommandBuffer) const;

    void ClearSingleUseCommands();

    operator VkCommandPool() const
    {
        return vkCommandPool;
    }

    VkQueue GetQueue() const
    {
        return m_queue;
    }

 private:
    VkCommandBuffer CreateSingleUseCommandBuffer();
    void DestroySingleUseCommandBuffer(VkCommandBuffer CommandBuffer);

 public:
    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> vkCommandBuffers;

 private:
    std::vector<VkCommandBuffer> m_singleUseVkCommandBuffers;

    VkQueue m_queue;
};

/* Creates new command buffer in pool and disposes it after going out of scope.
 * Move compatible
 * Begin and end buffer will be called automatically
 */
struct SingleUseCommandBuffer
{
    std::optional<VkCommandBuffer> buffer = {};

    explicit SingleUseCommandBuffer(
        CommandPool &CommandPool,
        const VkCommandBufferUsageFlags Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        const bool SubmitOnDestruct = true)
        : m_commandPool(CommandPool), m_submit(SubmitOnDestruct)
    {
        int32_t buffer_index = 0;
        buffer = m_commandPool.CreateSingleUseCommandBuffer();
        m_commandPool.BeginCommandBuffer(buffer.value(), true, true, Flags);
    }

    ~SingleUseCommandBuffer() noexcept
    {
        if (buffer.has_value())
			m_commandPool.DestroySingleUseCommandBuffer(buffer.value());
    }

    SingleUseCommandBuffer(const SingleUseCommandBuffer &Other) = delete;

    SingleUseCommandBuffer(SingleUseCommandBuffer &&Other) noexcept
        : m_commandPool(Other.m_commandPool), m_submit(Other.m_submit)
    {
        buffer = Other.buffer;
        Other.buffer.reset();
    }
    
    void Submit()
    {
        if (!m_submitted && buffer.has_value()) {
            m_commandPool.EndCommandBuffer(buffer.value());
            if (m_submit) {
                VkSubmitInfo submit_info{};
                submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit_info.commandBufferCount = 1;
                submit_info.pCommandBuffers = &buffer.value();

                vkQueueSubmit(m_commandPool.GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
                vkQueueWaitIdle(m_commandPool.GetQueue());
            }

            m_submitted = true;
        }
    }

    [[nodiscard]] VkCommandBuffer &Get()
    {
        return buffer.value();
    }

    [[nodiscard]] operator VkCommandBuffer() const
    {
        return buffer.value();
    }

 private:
    CommandPool &m_commandPool;
    bool m_submit;
    bool m_submitted = false;
};