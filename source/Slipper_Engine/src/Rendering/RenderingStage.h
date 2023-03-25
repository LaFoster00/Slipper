#pragma once

namespace Slipper
{
	class Mesh;
	class Shader;
	class Texture2D;
class OffscreenSwapChain;
class SurfaceSwapChain;
class SwapChain;
class RenderPass;
class CommandPool;

using SwapChainVariants = std::variant<OwningPtr<SurfaceSwapChain>, OwningPtr<OffscreenSwapChain>>;

class RenderingStage
{
 public:
    // use like "new RenderingStage(
    //      "RenderingStage",
    //      swapChain,
    //      device.graphicsQueue,
    //      device.queueFamilyIndices.graphicsFamily.value(),
    //      Engine::MAX_FRAMES_IN_FLIGHT)
    RenderingStage(std::string Name,
                   SwapChainVariants SwapChain,
                   VkQueue CommandQueue,
                   uint32_t CommandQueueFamilyIndex,
                   int32_t CommandBufferCount = Engine::MAX_FRAMES_IN_FLIGHT);
    ~RenderingStage();

    VkCommandBuffer BeginRender(uint32_t ImageIndex);
    void EndRender();

    void SubmitDraw(NonOwningPtr<const RenderPass> RenderPass,
                    NonOwningPtr<const Shader>Shader,
                    NonOwningPtr<const Mesh> Mesh,
                    const glm::mat4 &Transform);
    void SubmitSingleDrawCommand(const RenderPass *RP,
                                 std::function<void(const VkCommandBuffer &)> Command);
    void SubmitRepeatedDrawCommand(
        const RenderPass *RP,
        std::function<void(const VkCommandBuffer &, const RenderPass &)> Command);

    void RegisterForRenderPass(NonOwningPtr<RenderPass> RenderPass);
    void ChangeResolution(uint32_t Width, uint32_t Height);
    bool HasPresentationTextures() const;
    NonOwningPtr<Texture2D> GetPresentationTexture(uint32_t ImageIndex) const;
    NonOwningPtr<SwapChain> GetSwapChain();

    CommandPool &GetCommandPool() const
    {
        return *commandPool;
    }

 public:
    std::string name;
    SwapChainVariants swapChain;
    std::unordered_set<NonOwningPtr<RenderPass>> renderPasses;

    // Draw Commands
    OwningPtr<CommandPool> commandPool;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleCommands;
    std::unordered_map<
        NonOwningPtr<const RenderPass>,
        std::vector<std::function<void(const VkCommandBuffer &, const RenderPass &)>>>
        repeatedCommands;

 private:
    uint32_t m_currentImageIndex;
};
}  // namespace Slipper