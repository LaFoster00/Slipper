#pragma once

namespace Slipper
{
class RenderingStage;
class Framebuffer;
class Shader;
class Device;
class SwapChain;
class GraphicsPipeline;

class RenderPass : DeviceDependentObject
{
 public:
    RenderPass() = delete;
    RenderPass(std::string_view Name,
               VkFormat RenderingFormat,
               VkFormat DepthFormat,
               bool ForPresentation = true);
    ~RenderPass();

    void DestroyAllFrameBuffers();
    bool DestroySwapChainFramebuffers(NonOwningPtr<SwapChain> SwapChain);
    void CreateSwapChainFramebuffers(NonOwningPtr<SwapChain> SwapChain);

    void RecreateSwapChainResources(SwapChain *SwapChain);
    
    void BeginRenderPass(SwapChain *SwapChain, uint32_t ImageIndex, VkCommandBuffer CommandBuffer);
    void EndRenderPass(VkCommandBuffer commandBuffer);

    void RegisterShader(Shader *Shader);
    void UnregisterShader(Shader *Shader);

    [[nodiscard]] SwapChain *GetActiveSwapChain() const
    {
        return m_activeSwapChain;
    }

    operator VkRenderPass() const
    {
        return vkRenderPass;
    }

 public:
    std::string name;
    VkRenderPass vkRenderPass;

    std::unordered_map<SwapChain *, std::vector<std::unique_ptr<Framebuffer>>>
        swapChainFramebuffers;

    std::unordered_set<NonOwningPtr<Shader>> registeredShaders;
    std::unordered_set<NonOwningPtr<RenderingStage>> registeredRenderingStages;

 private:
    SwapChain *m_activeSwapChain;
};
}  // namespace Slipper