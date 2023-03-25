#include "RenderingStage.h"

#include "CameraComponent.h"
#include "Drawing/CommandPool.h"
#include "Mesh/UniformBuffer.h"
#include "Model/Model.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Presentation/SurfaceSwapChain.h"
#include "RenderPass.h"
#include "Shader/Shader.h"
#include "Texture/Texture2D.h"

namespace Slipper
{
RenderingStage::RenderingStage(std::string Name,
                               SwapChainVariants SwapChain,
                               VkQueue CommandQueue,
                               uint32_t CommandQueueFamilyIndex,
                               const bool NativeSwapChain,
                               int32_t CommandBufferCount)
    : name(Name), swapChain(std::move(SwapChain)), m_nativeSwapChain(NativeSwapChain)
{
    commandPool = new CommandPool(CommandQueue, CommandQueueFamilyIndex, CommandBufferCount);
}

RenderingStage::~RenderingStage()
{
    for (const auto render_pass : renderPasses) {
        render_pass->DestroySwapChainFramebuffers(GetSwapChain());
    }
    renderPasses.clear();
}

VkCommandBuffer RenderingStage::BeginRender()
{
    if (m_nativeSwapChain) {
        const auto result = GetSwapChain<SurfaceSwapChain>()->AcquireNextImageKhr();

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // This should be handled by even OnWindowResize
            // current_surface->RecreateSwapChain();
            ASSERT(1, "This should not be reached.")
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }
    }
    const auto draw_command_buffer = commandPool->vkCommandBuffers[GetCurrentImageIndex()];
    commandPool->BeginCommandBuffer(draw_command_buffer);

    for (const auto render_pass : renderPasses) {
        render_pass->BeginRenderPass(GetSwapChain(), GetCurrentImageIndex(), draw_command_buffer);
    }

    return draw_command_buffer;
}

void RenderingStage::EndRender()
{
    const auto draw_command_buffer = commandPool->vkCommandBuffers[GetCurrentImageIndex()];

    for (auto render_pass : renderPasses) {
        for (auto &single_draw_command : singleCommands[render_pass]) {
            single_draw_command(draw_command_buffer);
        }

        singleCommands.at(render_pass).clear();

        for (auto &repeated_draw_command : repeatedCommands[render_pass]) {
            repeated_draw_command(draw_command_buffer, *render_pass);
        }

        render_pass->EndRenderPass(draw_command_buffer);

        if (HasPresentationTextures()) {
            const auto [width, height] = GetSwapChain()->GetResolution();
            GetPresentationTexture()->EnqueueCopyImage(draw_command_buffer,
                                                       GetSwapChain()->GetCurrentSwapChainImage(),
                                                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                       {width, height, 1},
                                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }
    commandPool->EndCommandBuffer(draw_command_buffer);
}

void RenderingStage::SubmitDraw(NonOwningPtr<const RenderPass> RenderPass,
                                NonOwningPtr<const Shader> Shader,
                                NonOwningPtr<const Mesh> Mesh,
                                const glm::mat4 &Transform)
{
    SubmitSingleDrawCommand(RenderPass, [=, this](const VkCommandBuffer &CommandBuffer) {
        Shader->Use(CommandBuffer, RenderPass);

        UniformVP vp;
        const auto resolution = GetSwapChain()->GetResolution();
        auto [view, projection] = Camera::Parameters::GetViewProjection(
            GraphicsEngine::Get().GetDefaultCamera(), resolution.width / resolution.height);
        vp.view = view;
        vp.projection = projection;

        UniformModel model;
        model.model = Transform;

        vp.projection[1][1] *= -1;
        Shader->GetUniformBuffer("vp")->SubmitData(&vp);
        Shader->GetUniformBuffer("m")->SubmitData(&model);
        Mesh->Bind(CommandBuffer);
        vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(Mesh->NumIndex()), 1, 0, 0, 0);
    });
}

void RenderingStage::SubmitSingleDrawCommand(const RenderPass *RP,
                                             std::function<void(const VkCommandBuffer &)> Command)
{
    singleCommands[RP].emplace_back(Command);
}

void RenderingStage::SubmitRepeatedDrawCommand(
    const RenderPass *RP, std::function<void(const VkCommandBuffer &, const RenderPass &)> Command)
{
    repeatedCommands[RP].emplace_back(Command);
}

void RenderingStage::RegisterForRenderPass(NonOwningPtr<RenderPass> RenderPass)
{
    if (renderPasses.contains(RenderPass))
        return;

    RenderPass->CreateSwapChainFramebuffers(GetSwapChain());
    renderPasses.insert(RenderPass);
}

void RenderingStage::UnregisterFromRenderPass(NonOwningPtr<RenderPass> RenderPass)
{
    if (renderPasses.contains(RenderPass))
        renderPasses.erase(RenderPass);
    RenderPass->DestroySwapChainFramebuffers(GetSwapChain());
}

void RenderingStage::ChangeResolution(uint32_t Width, uint32_t Height)
{
    GetSwapChain()->Recreate(Width, Height);
    for (auto render_pass : renderPasses) {
        render_pass->RecreateSwapChainResources(GetSwapChain());
    }
}

bool RenderingStage::HasPresentationTextures() const
{
    if (std::holds_alternative<OwningPtr<OffscreenSwapChain>>(swapChain)) {
        const auto &offscreenSwapChain = std::get<OwningPtr<OffscreenSwapChain>>(swapChain);
        return offscreenSwapChain->withPresentationTextures;
    }
    return false;
}

NonOwningPtr<Texture2D> RenderingStage::GetPresentationTexture() const
{
    if (std::holds_alternative<OwningPtr<OffscreenSwapChain>>(swapChain)) {
        const auto &offscreenSwapChain = std::get<OwningPtr<OffscreenSwapChain>>(swapChain);
        return offscreenSwapChain->presentationTextures[GetCurrentImageIndex()];
    }
    return nullptr;
}

NonOwningPtr<SwapChain> RenderingStage::GetSwapChain()
{
    return std::visit(
        [](auto const &Sc) -> SwapChain * { return static_cast<SwapChain *>(Sc.get()); },
        swapChain);
}

uint32_t RenderingStage::GetCurrentImageIndex() const
{
    if (m_nativeSwapChain)
        return GetSwapChain<SurfaceSwapChain>()->GetCurrentSwapChainImageIndex();
    else
        return GraphicsEngine::Get().GetCurrentFrame();
}

template <>
NonOwningPtr<OffscreenSwapChain> RenderingStage::GetSwapChain<OffscreenSwapChain>() const
{
	return std::get<OwningPtr<OffscreenSwapChain>>(swapChain);
}

template <>
NonOwningPtr<SurfaceSwapChain> RenderingStage::GetSwapChain<SurfaceSwapChain>() const
{
    return std::get<NonOwningPtr<SurfaceSwapChain>>(swapChain);
}

VkSemaphore RenderingStage::GetCurrentImageAvailableSemaphore() const
{
    return GetSwapChain<SurfaceSwapChain>()
        ->m_imageAvailableSemaphores[GraphicsEngine::Get().GetCurrentFrame()];
}

VkSemaphore RenderingStage::GetCurrentRenderFinishSemaphore() const
{
    return GetSwapChain<SurfaceSwapChain>()
        ->m_renderFinishedSemaphores[GraphicsEngine::Get().GetCurrentFrame()];
}
}  // namespace Slipper
