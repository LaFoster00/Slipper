#include "../vk_RenderingStage.h"


namespace Slipper::GPU::Vulkan
{
    VKRenderingStage::VKRenderingStage(std::string Name, NonOwningPtr<SwapChain> SwapChain, const bool NativeSwapChain)
        : name(Name), swapChain(std::move(SwapChain)), m_nativeSwapChain(NativeSwapChain)
    {
        graphicsCommandPool = new CommandPool(
            device.graphicsQueue, device.queueFamilyIndices.graphicsFamily.value(), Engine::MAX_FRAMES_IN_FLIGHT);
        computeCommandPool = new CommandPool(
            device.computeQueue, device.queueFamilyIndices.computeFamily.value(), Engine::MAX_FRAMES_IN_FLIGHT);
        m_computeFinishedSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);

        constexpr vk::SemaphoreCreateInfo semaphore_create_info{vk::SemaphoreCreateFlags()};
        for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VK_HPP_ASSERT(
                device.logicalDevice.createSemaphore(&semaphore_create_info, nullptr, &m_computeFinishedSemaphores[i]),
                "Failed to create compute semaphore!");
        }
    }

    VKRenderingStage::~VKRenderingStage()
    {
        for (const auto compute_finished_semaphore : m_computeFinishedSemaphores)
        {
            device.logicalDevice.destroySemaphore(compute_finished_semaphore);
        }

        renderPasses.clear();
    }

    void VKRenderingStage::BeginRender() const
    {
        if (m_nativeSwapChain)
        {
            const auto result = TryGetSwapChain<SurfaceSwapChain>()->AcquireNextImageKhr();

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                // This should be handled by even OnWindowResize
                // current_surface->RecreateSwapChain();
                ASSERT(1, "This should not be reached.")
            }
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                throw std::runtime_error("Failed to acquire swap chain image!");
            }
        }

        const auto draw_command_buffer = graphicsCommandPool->BeginCurrentCommandBuffer();
        for (const auto render_pass : renderPasses)
        {
            render_pass->BeginRenderPass(GetSwapChain(), GetCurrentImageIndex(), draw_command_buffer);
        }

        computeCommandPool->BeginCurrentCommandBuffer();
    }

    void VKRenderingStage::EndRender()
    {
        const auto draw_command_buffer = graphicsCommandPool->GetCurrentCommandBuffer();
        const auto compute_command_buffer = computeCommandPool->GetCurrentCommandBuffer();

        for (auto render_pass : renderPasses)
        {
            // Execute all compute commands
            for (auto &repeated_compute_command : repeatedComputeCommands[render_pass])
            {
                repeated_compute_command(compute_command_buffer);
            }
            for (auto &single_compute_command : singleComputeCommands[render_pass])
            {
                single_compute_command(draw_command_buffer);
            }
            singleComputeCommands.at(render_pass).clear();

            // Execute all graphics commands
            for (auto &repeated_draw_command : repeatedGraphicsCommands[render_pass])
            {
                repeated_draw_command(draw_command_buffer);
            }
            for (auto &single_draw_command : singleGraphicsCommands[render_pass])
            {
                single_draw_command(draw_command_buffer);
            }
            singleGraphicsCommands.at(render_pass).clear();

            render_pass->EndRenderPass(draw_command_buffer);

            if (HasPresentationTextures())
            {
                const auto [width, height] = GetSwapChain()->GetResolution();
                GetPresentationTexture()->EnqueueCopyImage(draw_command_buffer,
                                                           GetSwapChain()->GetCurrentSwapChainImage(),
                                                           vk::ImageLayout::eTransferSrcOptimal,
                                                           {width, height, 1},
                                                           vk::ImageLayout::eShaderReadOnlyOptimal);
            }
        }
        computeCommandPool->EndCommandBuffer(compute_command_buffer);
        graphicsCommandPool->EndCommandBuffer(draw_command_buffer);
    }

    void VKRenderingStage::SubmitSingleComputeCommand(const RenderPass *RP,
                                                    std::function<void(const VkCommandBuffer &)> Command)
    {
        singleComputeCommands[RP].emplace_back(Command);
    }

    void VKRenderingStage::SubmitRepeatedComputeCommand(const RenderPass *RP,
                                                      std::function<void(const VkCommandBuffer &)> Command)
    {
        repeatedComputeCommands[RP].emplace_back(Command);
    }

    void VKRenderingStage::SubmitDraw(NonOwningPtr<const RenderPass> RenderPass,
                                    NonOwningPtr<const Material> Material,
                                    NonOwningPtr<const Model> Model,
                                    const glm::mat4 &Transform)
    {
        SubmitSingleDrawCommand(RenderPass,
                                [=, this](const VkCommandBuffer &CommandBuffer)
                                {
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

    void VKRenderingStage::SubmitSingleDrawCommand(const RenderPass *RP,
                                                 std::function<void(const VkCommandBuffer &)> Command)
    {
        singleGraphicsCommands[RP].emplace_back(Command);
    }

    void VKRenderingStage::SubmitRepeatedDrawCommand(const RenderPass *RP,
                                                   std::function<void(const VkCommandBuffer &)> Command)
    {
        repeatedGraphicsCommands[RP].emplace_back(Command);
    }

    void VKRenderingStage::RegisterForRenderPass(NonOwningPtr<RenderPass> RenderPass)
    {
        if (renderPasses.contains(RenderPass))
            return;

        GetSwapChain()->CreateFramebuffers(RenderPass);
        renderPasses.insert(RenderPass);
    }

    void VKRenderingStage::UnregisterFromRenderPass(NonOwningPtr<RenderPass> RenderPass)
    {
        if (renderPasses.contains(RenderPass))
            renderPasses.erase(RenderPass);
        GetSwapChain()->DestroyFramebuffers(RenderPass);
    }

    void VKRenderingStage::ChangeResolution(uint32_t Width, uint32_t Height)
    {
        GetSwapChain()->Recreate(Width, Height);
        LOG_FORMAT(
            "Swapchain for Rendering Stage '{}' has been recreated with a resoltion of [{},{}]", name, Width, Height);
    }

    bool VKRenderingStage::HasPresentationTextures() const
    {
        if (IsSwapChain<OffscreenSwapChain>())
        {
            return TryGetSwapChain<OffscreenSwapChain>()->withPresentationTextures;
        }
        return false;
    }

    NonOwningPtr<Texture2D> VKRenderingStage::GetPresentationTexture() const
    {
        if (IsSwapChain<OffscreenSwapChain>())
        {
            return TryGetSwapChain<OffscreenSwapChain>()->presentationTextures[GetCurrentImageIndex()];
        }
        return nullptr;
    }

    NonOwningPtr<SwapChain> VKRenderingStage::GetSwapChain() const
    {
        return swapChain;
    }

    uint32_t VKRenderingStage::GetCurrentImageIndex() const
    {
        if (m_nativeSwapChain)
            return TryGetSwapChain<SurfaceSwapChain>()->GetCurrentSwapChainImageIndex();
        else
            return GraphicsEngine::Get().GetCurrentFrame();
    }

    VkSemaphore VKRenderingStage::GetCurrentImageAvailableSemaphore() const
    {
        return TryGetSwapChain<SurfaceSwapChain>()->m_imageAvailableSemaphores[GraphicsEngine::Get().GetCurrentFrame()];
    }

    VkSemaphore VKRenderingStage::GetCurrentRenderFinishSemaphore() const
    {
        return TryGetSwapChain<SurfaceSwapChain>()->m_renderFinishedSemaphores[GraphicsEngine::Get().GetCurrentFrame()];
    }

    VkSemaphore VKRenderingStage::GetCurrentComputeFinishedSemaphore() const
    {
        return m_computeFinishedSemaphores[GraphicsEngine::Get().GetCurrentFrame()];
    }
}  // namespace Slipper
