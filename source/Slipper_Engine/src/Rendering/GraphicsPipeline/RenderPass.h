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
               vk::Format RenderingFormat,
               vk::Format DepthFormat,
               bool ForPresentation = true);
    ~RenderPass();
    
    void BeginRenderPass(SwapChain *SwapChain, uint32_t ImageIndex, VkCommandBuffer CommandBuffer);
    void EndRenderPass(VkCommandBuffer commandBuffer);

    [[nodiscard]] SwapChain *GetActiveSwapChain() const
    {
        return m_activeSwapChain;
    }

    operator VkRenderPass() const
    {
        return vkRenderPass;
    }

    operator vk::RenderPass() const
    {
        return vkRenderPass;
    }

 public:
    std::string name;
    VkRenderPass vkRenderPass;
    
    std::unordered_set<NonOwningPtr<RenderingStage>> registeredRenderingStages;

 private:
    SwapChain *m_activeSwapChain;
};
}  // namespace Slipper