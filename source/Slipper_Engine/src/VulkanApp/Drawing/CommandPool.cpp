#include "CommnadPool.h"

#include "../Setup/Device.h"
#include "../GraphicsPipeline/RenderPass.h"

CommandPool::CommandPool(Device &device) : owningDevice(device)
{
    QueueFamilyIndices &queueFamilyIndices = device.queueFamilyIndices;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VK_ASSERT(vkCreateCommandPool(device.logicalDevice, &poolInfo, nullptr, &vkCommandPool), "Failed to create command pool");
}

void CommandPool::Destroy()
{
    vkDestroyCommandPool(owningDevice.logicalDevice, vkCommandPool, nullptr);
}

VkCommandBuffer CommandPool::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    auto &commandBuffer = vkCommandBuffers.emplace_back();

    VK_ASSERT(vkAllocateCommandBuffers(owningDevice.logicalDevice, &allocInfo, vkCommandBuffers.data()), "Failed to create command buffers!");
    return commandBuffer;
}

void CommandPool::BeginCommandBuffer(RenderPass *renderPass, SwapChain *swapChain, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->vkRenderPass;
    renderPassInfo.framebuffer = renderPass->vkFramebuffers[swapChain][imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}