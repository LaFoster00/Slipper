#pragma once

#include "Framebuffer.h"
#include "common_includes.h"

class Device;
class RenderPass;

class CommandPool
{
 public:
    CommandPool() = delete;
    CommandPool(Device &device, int32_t BufferCount);
    ~CommandPool();

    VkCommandBuffer BeginCommandBuffer(uint32_t bufferIndex,
                                       bool resetCommandBuffer = true,
                                       VkCommandBufferUsageFlags flags = 0);
    VkCommandBuffer BeginCommandBuffer(VkCommandBuffer commandBuffer,
                                       bool resetCommandBuffer = true,
                                       VkCommandBufferUsageFlags flags = 0);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);
    void EndCommandBuffer(uint32_t bufferIndex);

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

        for (auto commandBuffer : CommandBuffers) {
            auto bufferLoc = std::find(vkCommandBuffers.begin(), vkCommandBuffers.end(), commandBuffer);
            if (bufferLoc != vkCommandBuffers.end())
            {
                vkCommandBuffers.erase(bufferLoc);
            }
        }
    }

 private:

 public:
    Device &device;

    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> vkCommandBuffers;
};