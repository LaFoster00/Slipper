#pragma once
#include "CommandPool.h"
#include "Object.h"
#include "Vulkan/vk_RenderPass.h"

namespace Slipper::GPU
{
    class RenderingStage : public Object<RenderingStage>
    {
     public:
        virtual ~RenderingStage() override = default;
        virtual std::string_view GetName() = 0;
        virtual uint32_t GetCurrentImageIndex() = 0;
        virtual NonOwningPtr<CommandPool> GetGraphicsCommandPool() = 0;
        virtual void RegisterForRenderPass(NonOwningPtr<Vulkan::RenderPass> non_owning) = 0;
        virtual void BeginRender() = 0;
        virtual void EndRender() = 0;
        virtual vk::Semaphore GetCurrentComputeFinishedSemaphore() = 0;
        virtual NonOwningPtr<CommandPool> GetComputeCommandPool() = 0;
        virtual bool IsPresentStage() = 0;
        virtual vk::Semaphore GetCurrentImageAvailableSemaphore() = 0;
        virtual vk::Semaphore GetCurrentRenderFinishSemaphore() = 0;
        virtual const NonOwningPtr<Vulkan::SwapChain> GetSwapChain() = 0;
        virtual void ChangeResolution(uint32_t width, uint32_t uint32) = 0;
        virtual void SubmitSingleComputeCommand(const Vulkan::RenderPass *RP,
                                                std::function<void(const VkCommandBuffer &)> Command) = 0;
        virtual void SubmitRepeatedComputeCommand(const Vulkan::RenderPass *RP,
                                                  std::function<void(const VkCommandBuffer &)> Command) = 0;
        virtual void SubmitDraw(NonOwningPtr<const Vulkan::RenderPass> RenderPass,
                                NonOwningPtr<const Material> Material,
                                NonOwningPtr<const Model> Model,
                                const glm::mat4 &Transform) = 0;
        virtual void SubmitSingleDrawCommand(const Vulkan::RenderPass *RP,
                                             std::function<void(const VkCommandBuffer &)> Command) = 0;
        virtual void SubmitRepeatedDrawCommand(const Vulkan::RenderPass *RP,
                                               std::function<void(const VkCommandBuffer &)> Command) = 0;
    };
}  // namespace Slipper::GPU
