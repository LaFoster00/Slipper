#include "RenderingStage.h"

#include "CameraComponent.h"
#include "Material.h"
#include "Drawing/CommandPool.h"
#include "Model/Model.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Presentation/SurfaceSwapChain.h"
#include "RenderPass.h"
#include "Buffer/UniformBuffer.h"
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
    const auto draw_command_buffer =
        commandPool->vkCommandBuffers[GraphicsEngine::Get().GetCurrentFrame()];
    commandPool->BeginCommandBuffer(draw_command_buffer);

    for (const auto render_pass : renderPasses) {
        render_pass->BeginRenderPass(GetSwapChain(), GetCurrentImageIndex(), draw_command_buffer);
    }

    return draw_command_buffer;
}

void RenderingStage::EndRender()
{
    const auto draw_command_buffer =
        commandPool->vkCommandBuffers[GraphicsEngine::Get().GetCurrentFrame()];

    for (auto render_pass : renderPasses) {
        for (auto &repeated_draw_command : repeatedCommands[render_pass]) {
            repeated_draw_command(draw_command_buffer, *render_pass);
        }

        for (auto &single_draw_command : singleCommands[render_pass]) {
            single_draw_command(draw_command_buffer);
        }

        singleCommands.at(render_pass).clear();

        render_pass->EndRenderPass(draw_command_buffer);

        if (HasPresentationTextures()) {
            const auto [width, height] = GetSwapChain()->GetResolution();
            GetPresentationTexture()->EnqueueCopyImage(draw_command_buffer,
                                                       GetSwapChain()->GetCurrentSwapChainImage(),
                                                       vk::ImageLayout::eTransferSrcOptimal,
                                                       {width, height, 1},
                                                       vk::ImageLayout::eShaderReadOnlyOptimal);
        }
    }
    commandPool->EndCommandBuffer(draw_command_buffer);
}

void RenderingStage::SubmitDraw(NonOwningPtr<const RenderPass> RenderPass,
                                NonOwningPtr<const Material> Material,
                                NonOwningPtr<const Model> Model,
                                const glm::mat4 &Transform)
{
    SubmitSingleDrawCommand(RenderPass, [=, this](const VkCommandBuffer &CommandBuffer) {
	    const auto resolution = GetSwapChain()->GetResolution();
        Material->Use(CommandBuffer, RenderPass, resolution);

        const auto camera = GraphicsEngine::GetDefaultCamera();
	    const auto &cam_parameters = camera.GetComponent<Camera>();

        UniformVP vp;
        vp.view = cam_parameters.GetView();
        vp.projection = cam_parameters.GetProjection(static_cast<float>(resolution.width) /
                                                     resolution.height);

        UniformModel model;
        model.model = Transform;

        Material->GetUniformBuffer("vp")->SubmitData(&vp);
        Material->GetUniformBuffer("m")->SubmitData(&model);

        Model->Draw(CommandBuffer);
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

    GetSwapChain()->CreateFramebuffers(RenderPass);
    renderPasses.insert(RenderPass);
}

void RenderingStage::UnregisterFromRenderPass(NonOwningPtr<RenderPass> RenderPass)
{
    if (renderPasses.contains(RenderPass))
        renderPasses.erase(RenderPass);
    GetSwapChain()->DestroyFramebuffers(RenderPass);
}

void RenderingStage::ChangeResolution(uint32_t Width, uint32_t Height)
{
    GetSwapChain()->Recreate(Width, Height);
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

template<>
NonOwningPtr<OffscreenSwapChain> RenderingStage::GetSwapChain<OffscreenSwapChain>() const
{
    return std::get<OwningPtr<OffscreenSwapChain>>(swapChain);
}

template<> NonOwningPtr<SurfaceSwapChain> RenderingStage::GetSwapChain<SurfaceSwapChain>() const
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
