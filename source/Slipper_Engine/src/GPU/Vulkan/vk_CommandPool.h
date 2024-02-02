#pragma once
#include "CommandPool.h"
#include "vk_DeviceDependentObject.h"

namespace Slipper::GPU::Vulkan
{
    struct SingleUseCommandBuffer;
    class RenderPass;

    class CommandPool : DeviceDependentObject, public GPU::CommandPool
    {
        friend SingleUseCommandBuffer;

     public:
        explicit CommandPool(vk::Queue Queue, uint32_t QueueFamilyIndex, int32_t BufferCount = 0);
        ~CommandPool();

        std::vector<vk::CommandBuffer> &CreateCommandBuffers(int32_t BufferCount,
                                                             int32_t *NewCommandBufferStartIndex = nullptr);
        void DestroyCommandBuffers(const std::vector<vk::CommandBuffer> &CommandBuffers);

        void BeginCommandBuffer(vk::CommandBuffer CommandBuffer,
                                bool ResetCommandBuffer = true,
                                bool SingleUseBuffer = false,
                                vk::CommandBufferUsageFlags Flags = {});
        void EndCommandBuffer(vk::CommandBuffer CommandBuffer) const;

        void ClearSingleUseCommands();

        operator VkCommandPool() const
        {
            return m_vkCommandPool;
        }

        [[nodiscard]] vk::Queue GetQueue() const
        {
            return m_queue;
        }

        [[nodiscard]] vk::CommandBuffer GetCurrentCommandBuffer() const;

        vk::CommandBuffer BeginCurrentCommandBuffer()
        {
            const auto command_buffer = GetCurrentCommandBuffer();
            BeginCommandBuffer(command_buffer);
            return command_buffer;
        }

        vk::CommandBuffer EndCurrentCommandBuffer() const
        {
            const auto command_buffer = GetCurrentCommandBuffer();
            EndCommandBuffer(command_buffer);
            return command_buffer;
        }

     private:
        vk::CommandBuffer CreateSingleUseCommandBuffer();
        void DestroySingleUseCommandBuffer(vk::CommandBuffer CommandBuffer);

        vk::CommandPool m_vkCommandPool;
        std::vector<vk::CommandBuffer> m_vkCommandBuffers;

     private:
        std::vector<vk::CommandBuffer> m_singleUseVkCommandBuffers;

        vk::Queue m_queue;
    };

    /* Creates new command buffer in pool and disposes it after going out of scope.
     * Move compatible
     * Begin and end buffer will be called automatically
     */
    struct SingleUseCommandBuffer
    {
        std::optional<vk::CommandBuffer> buffer = {};

        explicit SingleUseCommandBuffer(
            CommandPool &CommandPool,
            const vk::CommandBufferUsageFlags Flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
            : m_commandPool(CommandPool)
        {
            buffer = m_commandPool.CreateSingleUseCommandBuffer();
            m_commandPool.BeginCommandBuffer(buffer.value(), true, true, Flags);
        }

        explicit SingleUseCommandBuffer(const std::unique_ptr<GPU::CommandPool>::element_type & command_pool);

        ~SingleUseCommandBuffer() noexcept
        {
            if (buffer.has_value())
            {
                if (!m_submitted)
                {
                    DEBUG_BREAK
                }
                m_commandPool.DestroySingleUseCommandBuffer(buffer.value());
            }
        }

        SingleUseCommandBuffer(const SingleUseCommandBuffer &Other) = delete;

        SingleUseCommandBuffer(SingleUseCommandBuffer &&Other) noexcept : m_commandPool(Other.m_commandPool)
        {
            buffer = Other.buffer;
            Other.buffer.reset();
        }

        void Submit()
        {
            if (!m_submitted && buffer.has_value())
            {
                m_commandPool.EndCommandBuffer(buffer.value());

                const vk::SubmitInfo submit_info(0, nullptr, nullptr, 1, &buffer.value());
                m_commandPool.GetQueue().submit(submit_info);
                m_commandPool.GetQueue().waitIdle();

                m_submitted = true;
            }
        }

        [[nodiscard]] vk::CommandBuffer &Get()
        {
            return buffer.value();
        }

        [[nodiscard]] operator VkCommandBuffer() const
        {
            return buffer.value();
        }

        [[nodiscard]] operator vk::CommandBuffer() const
        {
            return buffer.value();
        }

     private:
        CommandPool &m_commandPool;
        bool m_submitted = false;
    };
}  // namespace Slipper::GPU::Vulkan
