#pragma once
#include "RenderingStage.h"
#include "vk_DeviceDependentObject.h"

namespace Slipper::GPU::Vulkan
{
    class VKRenderingStage : public RenderingStage, public DeviceDependentObject
    {
     public:
        // use like "new VKRenderingStage(
        //      "VKRenderingStage",
        //      swapChain,
        //      device.graphicsQueue,
        //      device.queueFamilyIndices.graphicsFamily.value(),
        //      Engine::MAX_FRAMES_IN_FLIGHT)
        VKRenderingStage(std::string Name, NonOwningPtr<VKSwapChain> SwapChain, bool NativeSwapChain);
        ~VKRenderingStage();

        void BeginRender() const;
        void EndRender();

        void SubmitSingleComputeCommand(const VKRenderPass *RP, std::function<void(const VkCommandBuffer &)> Command);

        void SubmitRepeatedComputeCommand(const VKRenderPass *RP, std::function<void(const VkCommandBuffer &)> Command);

        void SubmitDraw(NonOwningPtr<const VKRenderPass> RenderPass,
                        NonOwningPtr<const VKMaterial> Material,
                        NonOwningPtr<const VKModel> Model,
                        const glm::mat4 &Transform);
        void SubmitSingleDrawCommand(const VKRenderPass *RP, std::function<void(const VkCommandBuffer &)> Command);
        void SubmitRepeatedDrawCommand(const VKRenderPass *RP, std::function<void(const VkCommandBuffer &)> Command);

        void RegisterForRenderPass(NonOwningPtr<VKRenderPass> RenderPass);
        void UnregisterFromRenderPass(NonOwningPtr<VKRenderPass> RenderPass);
        void ChangeResolution(uint32_t Width, uint32_t Height);
        bool HasPresentationTextures() const;
        NonOwningPtr<VKTexture> GetPresentationTexture() const;
        NonOwningPtr<VKSwapChain> GetSwapChain() const;
        template<typename T>
        bool IsSwapChain() const
        {
            return swapChain.TryCast<T>() != nullptr;
        }

        template<typename T>
        NonOwningPtr<T> TryGetSwapChain() const
        {
            return swapChain.TryCast<T>();
        }

        uint32_t GetCurrentImageIndex() const;

        VkSemaphore GetCurrentImageAvailableSemaphore() const;
        VkSemaphore GetCurrentRenderFinishSemaphore() const;
        VkSemaphore GetCurrentComputeFinishedSemaphore() const;

        VKCommandPool &GetGraphicsCommandPool() const
        {
            return *graphicsCommandPool;
        }

        VKCommandPool &GetComputeCommandPool() const
        {
            return *computeCommandPool;
        }

        bool IsPresentStage() const
        {
            return m_nativeSwapChain;
        }

     public:
        std::string name;
        NonOwningPtr<VkSwapChain> swapChain;
        std::unordered_set<NonOwningPtr<VKRenderPass>> renderPasses;

        // Draw Commands
        OwningPtr<VKCommandPool> graphicsCommandPool;
        std::unordered_map<NonOwningPtr<const VKRenderPass>, std::vector<std::function<void(const VkCommandBuffer &)>>>
            singleGraphicsCommands;
        std::unordered_map<NonOwningPtr<const VKRenderPass>, std::vector<std::function<void(const VkCommandBuffer &)>>>
            repeatedGraphicsCommands;

        // Compute Commands
        OwningPtr<VKCommandPool> computeCommandPool;
        std::unordered_map<NonOwningPtr<const VKRenderPass>, std::vector<std::function<void(const VkCommandBuffer &)>>>
            singleComputeCommands;
        std::unordered_map<NonOwningPtr<const VKRenderPass>, std::vector<std::function<void(const VkCommandBuffer &)>>>
            repeatedComputeCommands;

     private:
        bool m_nativeSwapChain;

        std::vector<vk::Semaphore> m_computeFinishedSemaphores;
    };
}  // namespace Slipper::GPU
