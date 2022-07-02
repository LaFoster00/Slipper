#pragma once

#include "Drawing/Framebuffer.h"
#include "common_includes.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Shader;
class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass
{
 public:
    RenderPass() = delete;
    RenderPass(Device &device, VkFormat attachmentFormat);
    ~RenderPass();

    void DestroyAllFrameBuffers();
    bool DestroySwapChainFramebuffers(SwapChain *SwapChain);

    void CreateSwapChainFramebuffers(SwapChain *swapChain);

    void BeginRenderPass(SwapChain *swapChain, uint32_t imageIndex, VkCommandBuffer commandBuffer);
    void EndRenderPass(VkCommandBuffer commandBuffer);

    void RegisterShader(Shader *Shader);
    void UnregisterShader(Shader *Shader);

    [[nodiscard]] SwapChain *GetActiveSwapChain() const
    {
        return m_activeSwapChain;
    }

 public:
    Device &device;

    VkRenderPass vkRenderPass;

    std::unordered_map<SwapChain *, std::vector<std::unique_ptr<Framebuffer>>>
        swapChainFramebuffers;

    std::unordered_set<Shader *> registeredShaders;

private:
    SwapChain *m_activeSwapChain;
};