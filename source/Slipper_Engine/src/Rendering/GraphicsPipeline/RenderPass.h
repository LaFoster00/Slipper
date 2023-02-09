#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DeviceDependentObject.h"

namespace Slipper
{
class Framebuffer;
class Shader;
class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass : DeviceDependentObject
{
 public:
    RenderPass() = delete;
    RenderPass(std::string_view Name, VkFormat AttachmentFormat, VkFormat DepthFormat);
    ~RenderPass();

    void DestroyAllFrameBuffers();
    bool DestroySwapChainFramebuffers(SwapChain *SwapChain);

    void CreateSwapChainFramebuffers(SwapChain *SwapChain);

    void BeginRenderPass(SwapChain *SwapChain, uint32_t ImageIndex, VkCommandBuffer CommandBuffer);
    void EndRenderPass(VkCommandBuffer commandBuffer);

    void RegisterShader(Shader *Shader);
    void UnregisterShader(Shader *Shader);

    [[nodiscard]] SwapChain *GetActiveSwapChain() const
    {
        return m_activeSwapChain;
    }

 public:
    std::string name;
    VkRenderPass vkRenderPass;

    std::unordered_map<SwapChain *, std::vector<std::unique_ptr<Framebuffer>>>
        swapChainFramebuffers;

    std::unordered_set<Shader *> registeredShaders;

 private:
    SwapChain *m_activeSwapChain;
};
}  // namespace Slipper