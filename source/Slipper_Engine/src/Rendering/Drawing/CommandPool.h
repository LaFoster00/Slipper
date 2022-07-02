#pragma once

#include "Framebuffer.h"
#include "common_includes.h"

class Device;
class RenderPass;

class CommandPool
{
 public:
    CommandPool() = delete;
    CommandPool(Device &Device, int32_t BufferCount);
    ~CommandPool();

    VkCommandBuffer BeginCommandBuffer(uint32_t BufferIndex,
                                       bool ResetCommandBuffer = true,
                                       VkCommandBufferUsageFlags Flags = 0) const;
    VkCommandBuffer BeginCommandBuffer(VkCommandBuffer CommandBuffer,
                                       bool ResetCommandBuffer = true,
                                       VkCommandBufferUsageFlags Flags = 0);
	void EndCommandBuffer(VkCommandBuffer CommandBuffer) const;
    void EndCommandBuffer(uint32_t BufferIndex) const;

    operator VkCommandPool() const
    {
        return vkCommandPool;
    }

    std::vector<VkCommandBuffer> &CreateCommandBuffers(int32_t BufferCount);

    template<size_t Count>
    void DestroyCommandBuffers(std::array<VkCommandBuffer, Count> CommandBuffers)
    {
        vkFreeCommandBuffers(
            device, vkCommandPool, static_cast<int32_t>(Count), CommandBuffers.data());

        for (auto command_buffer : CommandBuffers) {
            auto buffer_loc = std::find(vkCommandBuffers.begin(), vkCommandBuffers.end(), command_buffer);
            if (buffer_loc != vkCommandBuffers.end())
            {
                vkCommandBuffers.erase(buffer_loc);
            }
        }
    }

 private:

 public:
    Device &device;

    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> vkCommandBuffers;
};