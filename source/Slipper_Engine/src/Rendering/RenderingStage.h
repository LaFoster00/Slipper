#pragma once

namespace Slipper
{
class Material;
class Model;
class OffscreenSwapChain;
class Mesh;
class Shader;
class Texture2D;
class SurfaceSwapChain;
class SwapChain;
class RenderPass;
class CommandPool;

class RenderingStage : public DeviceDependentObject
{
 public:
    // use like "new RenderingStage(
    //      "RenderingStage",
    //      swapChain,
    //      device.graphicsQueue,
    //      device.queueFamilyIndices.graphicsFamily.value(),
    //      Engine::MAX_FRAMES_IN_FLIGHT)
    RenderingStage(std::string Name, NonOwningPtr<SwapChain> SwapChain, bool NativeSwapChain);
    ~RenderingStage();

    void BeginRender() const;
    void EndRender();

    void SubmitSingleComputeCommand(const RenderPass *RP,
                                    std::function<void(const VkCommandBuffer &)> Command);

    void SubmitRepeatedComputeCommand(const RenderPass *RP,
                                      std::function<void(const VkCommandBuffer &)> Command);

    void SubmitDraw(NonOwningPtr<const RenderPass> RenderPass,
                    NonOwningPtr<const Material> Material,
                    NonOwningPtr<const Model> Model,
                    const glm::mat4 &Transform);
    void SubmitSingleDrawCommand(const RenderPass *RP,
                                 std::function<void(const VkCommandBuffer &)> Command);
    void SubmitRepeatedDrawCommand(const RenderPass *RP,
                                   std::function<void(const VkCommandBuffer &)> Command);

    void RegisterForRenderPass(NonOwningPtr<RenderPass> RenderPass);
    void UnregisterFromRenderPass(NonOwningPtr<RenderPass> RenderPass);
    void ChangeResolution(uint32_t Width, uint32_t Height);
    bool HasPresentationTextures() const;
    NonOwningPtr<Texture2D> GetPresentationTexture() const;
    NonOwningPtr<SwapChain> GetSwapChain() const;
    template<typename T> bool IsSwapChain() const
    {
        return swapChain.TryCast<T>() != nullptr;
    }

    template<typename T> NonOwningPtr<T> TryGetSwapChain() const
    {
        return swapChain.TryCast<T>();
    }

    uint32_t GetCurrentImageIndex() const;

    VkSemaphore GetCurrentImageAvailableSemaphore() const;
    VkSemaphore GetCurrentRenderFinishSemaphore() const;
    VkSemaphore GetCurrentComputeFinishedSemaphore() const;

    CommandPool &GetGraphicsCommandPool() const
    {
        return *graphicsCommandPool;
    }

    CommandPool &GetComputeCommandPool() const
    {
        return *computeCommandPool;
    }

    bool IsPresentStage() const
    {
        return m_nativeSwapChain;
    }

 public:
    std::string name;
    NonOwningPtr<SwapChain> swapChain;
    std::unordered_set<NonOwningPtr<RenderPass>> renderPasses;

    // Draw Commands
    OwningPtr<CommandPool> graphicsCommandPool;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleGraphicsCommands;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        repeatedGraphicsCommands;

    // Compute Commands
    OwningPtr<CommandPool> computeCommandPool;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        singleComputeCommands;
    std::unordered_map<NonOwningPtr<const RenderPass>,
                       std::vector<std::function<void(const VkCommandBuffer &)>>>
        repeatedComputeCommands;

 private:
    bool m_nativeSwapChain;

    std::vector<vk::Semaphore> m_computeFinishedSemaphores;
};
}  // namespace Slipper