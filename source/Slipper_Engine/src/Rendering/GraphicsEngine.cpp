
#include "GraphicsEngine.h"

#include "Drawing/CommandPool.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Mesh/Mesh.h"
#include "Mesh/UniformBuffer.h"
#include "Presentation/Surface.h"

#include "Core/Application.h"
#include "Core/SlipperObject/Camera.h"
#include "Core/Transform.h"
#include "Drawing/Sampler.h"
#include "Model/Model.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Shader/Shader.h"
#include "Texture/Texture2D.h"
#include "Time/Time.h"
#include "Window.h"

namespace Slipper
{
GraphicsEngine *GraphicsEngine::m_graphicsInstance = nullptr;

GraphicsEngine::GraphicsEngine()
{
    ASSERT(m_graphicsInstance, "Graphics Engine allready created!");
    m_graphicsInstance = this;
}

GraphicsEngine::~GraphicsEngine()
{
    viewportPresentationTextures.clear();
    viewportSwapChain.reset();

    renderPassNames.clear();
    renderPasses.clear();

    memoryCommandPool.reset();
    guiCommandPool.reset();
    drawCommandPool.reset();

    for (const auto image_available_semaphore : m_imageAvailableSemaphores) {
        vkDestroySemaphore(device, image_available_semaphore, nullptr);
    }

    for (const auto render_finished_semaphore : m_renderFinishedSemaphores) {
        vkDestroySemaphore(device, render_finished_semaphore, nullptr);
    }

    for (const auto in_flight_fence : m_inFlightFences) {
        vkDestroyFence(device, in_flight_fence, nullptr);
    }

    shaders.clear();

    models.clear();
    textures.clear();
}

void GraphicsEngine::Init()
{
    m_graphicsInstance = new GraphicsEngine();
    auto &device = Device::Get();

    m_graphicsInstance->drawCommandPool = std::make_unique<CommandPool>(
        device.graphicsQueue,
        device.queueFamilyIndices.graphicsFamily.value(),
        Engine::MAX_FRAMES_IN_FLIGHT);
    m_graphicsInstance->guiCommandPool = std::make_unique<CommandPool>(
        device.graphicsQueue,
        device.queueFamilyIndices.graphicsFamily.value(),
        Engine::MAX_FRAMES_IN_FLIGHT);

    m_graphicsInstance->memoryCommandPool = std::make_unique<CommandPool>(
        device.transferQueue ? device.transferQueue : device.graphicsQueue,
        device.transferQueue ? device.queueFamilyIndices.transferFamily.value() :
                               device.queueFamilyIndices.graphicsFamily.value());

    m_graphicsInstance->windowRenderPass = m_graphicsInstance->CreateRenderPass(
        "Window", SwapChain::swapChainFormat, Texture2D::FindDepthFormat(), true);

    m_graphicsInstance->viewportRenderPass = m_graphicsInstance->CreateRenderPass(
        "Viewport", Engine::TARGET_VIEWPORT_COLOR_FORMAT, Texture2D::FindDepthFormat(), false);

    m_graphicsInstance->CreateViewportSwapChain();

    m_graphicsInstance->SetupDebugResources();
    m_graphicsInstance->CreateSyncObjects();
}

void GraphicsEngine::Shutdown()
{
    vkDeviceWaitIdle(Device::Get());

    Sampler::DestroyDefaultSamplers();
    delete m_graphicsInstance;
    m_graphicsInstance = nullptr;
}

void GraphicsEngine::SetupDebugResources()
{
    // The default shader depends on these so initialize them first
    textures.emplace_back(Texture2D::LoadTexture(DEMO_TEXTURE_PATH, true));
    models.emplace_back(std::make_unique<Model>(DEMO_MODEL_PATH));

    /* Create shader for this pipeline. */
    std::vector<std::tuple<std::string_view, ShaderType>> shader_stages = {
        {"./EngineContent/Shaders/Spir-V/Basic.vert.spv", ShaderType::VERTEX},
        {"./EngineContent/Shaders/Spir-V/Basic.frag.spv", ShaderType::FRAGMENT}};

    shaders.emplace_back(std::make_unique<Shader>("BasicVertex", shader_stages));
    shaders[0]->SetShaderUniform("texSampler", *textures[0]);
}

void GraphicsEngine::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_imageAvailableSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(Engine::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
        VK_ASSERT(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &m_imageAvailableSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &m_renderFinishedSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(vkCreateFence(device, &fence_info, nullptr, &m_inFlightFences[i]),
                  "Failed to create fence!")
    }
}

RenderPass *GraphicsEngine::CreateRenderPass(const std::string &Name,
                                             const VkFormat RenderingFormat,
                                             const VkFormat DepthFormat,
                                             const bool ForPresentation)
{
    renderPasses[Name] = std::make_unique<RenderPass>(
        Name, RenderingFormat, DepthFormat, ForPresentation);
    renderPassNames[renderPasses[Name].get()] = Name;
    return renderPasses[Name].get();
}

void GraphicsEngine::DestroyRenderPass(RenderPass *RenderPass)
{
    if (renderPassNames.contains(RenderPass)) {
        const auto name = renderPassNames.at(RenderPass);
        renderPassNames.erase(RenderPass);
        renderPasses.erase(name);
    }
}

void GraphicsEngine::CreateViewportSwapChain() const
{
    m_graphicsInstance->viewportSwapChain = std::make_unique<OffscreenSwapChain>(
        Application::Get().window->GetSize(),
        Engine::TARGET_VIEWPORT_COLOR_FORMAT,
        device.SurfaceSwapChainImagesCount(&Application::Get().window->GetSurface()));

    m_graphicsInstance->viewportRenderPass->CreateSwapChainFramebuffers(
        m_graphicsInstance->viewportSwapChain.get());

    m_graphicsInstance->viewportPresentationTextures.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < m_graphicsInstance->viewportSwapChain->numImages; ++i) {
        m_graphicsInstance->viewportPresentationTextures.push_back(
            std::make_unique<Texture2D>(Application::Get().window->GetSize(),
                                        Engine::TARGET_VIEWPORT_COLOR_FORMAT,
                                        SwapChain::swapChainFormat,
                                        false));
    }
}

void GraphicsEngine::RecreateViewportSwapChain(uint32_t Width, uint32_t Height) const
{
    m_graphicsInstance->viewportSwapChain->Recreate(Width, Height);
    viewportRenderPass->RecreateSwapChainResources(viewportSwapChain.get());

    for (const auto &viewport_presentation_texture :
         m_graphicsInstance->viewportPresentationTextures) {
        viewport_presentation_texture->Resize({Width, Height, 1});
    }
}

Camera &GraphicsEngine::GetDefaultCamera()
{
    if (!m_defaultCamera)
        m_defaultCamera = new Camera();

    return *m_defaultCamera;
}

void GraphicsEngine::AddWindow(Window &Window)
{
    windows.insert(&Window);
    Window.GetSurface().CreateSwapChain();
    Window.GetSurface().RegisterRenderPass(*windowRenderPass);
}

void GraphicsEngine::SetupDebugRender(Surface &Surface)
{
    shaders[0]->RegisterForRenderPass(viewportRenderPass, Surface.GetResolution());

    SetupSimpleDraw();
}

void GraphicsEngine::SetupSimpleDraw()
{
    SubmitRepeatedDrawCommand(
        [=, this](const VkCommandBuffer &CommandBuffer, const RenderPass &RenderPass) {
            const auto debug_shader = *RenderPass.registeredShaders.begin();
            debug_shader->Bind(CommandBuffer, &RenderPass);

            Camera &camera = GetDefaultCamera();
            Transform transform({0, 0, 0}, {1, 1, 1}, {0, 0, Time::TimeSinceStartup() * 90.0f});

            UniformMVP mvp;
            mvp.model = transform.GetModelMatrix();
            mvp.view = camera.GetView();
            mvp.projection = camera.GetProjection(
                RenderPass.GetActiveSwapChain()->GetResolution().width /
                static_cast<float>(RenderPass.GetActiveSwapChain()->GetResolution().height));

            debug_shader->GetUniformBuffer("mvp")->SubmitData(&mvp);

            models[0]->Draw(CommandBuffer);
        });
}

void GraphicsEngine::SubmitDraw(const RenderPass *RenderPass,
                                const Shader *Shader,
                                const Mesh *Mesh,
                                const glm::mat4 &Transform)
{
    SubmitSingleDrawCommand(RenderPass, [=, this](const VkCommandBuffer &CommandBuffer) {
        Shader->Bind(CommandBuffer, RenderPass);
        UniformMVP mvp;
        mvp.model = Transform;
        mvp.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.projection = glm::perspective(
            glm::radians(45.0f),
            RenderPass->GetActiveSwapChain()->GetResolution().width /
                static_cast<float>(RenderPass->GetActiveSwapChain()->GetResolution().height),
            0.1f,
            10.0f);

        mvp.projection[1][1] *= -1;
        Shader->GetUniformBuffer("mvp")->SubmitData(&mvp);
        Mesh->Bind(CommandBuffer);
        vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(Mesh->NumIndex()), 1, 0, 0, 0);
    });
}

void GraphicsEngine::SubmitSingleDrawCommand(const RenderPass *RenderPass,
                                             std::function<void(const VkCommandBuffer &)> Command)
{
    singleDrawCommands[RenderPass].push_back(Command);
}

void GraphicsEngine::SubmitRepeatedDrawCommand(
    const std::function<void(const VkCommandBuffer &, const RenderPass &)> Command)
{
    repeatedDrawCommands.push_back(Command);
}

void GraphicsEngine::BeginUpdate()
{
    vkWaitForFences(device, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    m_currentSurface = &(*windows.begin())->GetSurface();
    auto result = vkAcquireNextImageKHR(device,
                                        *m_currentSurface->swapChain,
                                        UINT64_MAX,
                                        m_imageAvailableSemaphores[currentFrame],
                                        VK_NULL_HANDLE,
                                        &m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // This should be handled by even OnWindowResize
        // current_surface->RecreateSwapChain();
        ASSERT(1, "This should not be reached.")
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    const auto draw_command_buffer = drawCommandPool->vkCommandBuffers[currentFrame];
    drawCommandPool->BeginCommandBuffer(draw_command_buffer);

    viewportRenderPass->BeginRenderPass(
        viewportSwapChain.get(), m_currentImageIndex, draw_command_buffer);
}

void GraphicsEngine::EndUpdate()
{
    const auto draw_command_buffer = drawCommandPool->vkCommandBuffers[currentFrame];

    for (auto &single_draw_command : singleDrawCommands[viewportRenderPass]) {
        single_draw_command(draw_command_buffer);
    }

    singleDrawCommands.at(viewportRenderPass).clear();

    for (auto &repeated_draw_command : repeatedDrawCommands) {
        repeated_draw_command(draw_command_buffer, *viewportRenderPass);
    }

    viewportRenderPass->EndRenderPass(draw_command_buffer);

    {
        const auto viewport_resolution = viewportSwapChain->GetResolution();
        viewportPresentationTextures[m_currentImageIndex]->EnqueueCopyImage(
            draw_command_buffer,
            viewportRenderPass->GetCurrentImage(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            {viewport_resolution.width, viewport_resolution.height, 1},
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    drawCommandPool->EndCommandBuffer(draw_command_buffer);
}

void GraphicsEngine::BeginGuiUpdate()
{
    const auto gui_command_buffer = guiCommandPool->vkCommandBuffers[currentFrame];
    guiCommandPool->BeginCommandBuffer(gui_command_buffer);
    m_currentRenderPass = windowRenderPass;
    m_currentRenderPass->BeginRenderPass(
        m_currentSurface->swapChain.get(), m_currentImageIndex, gui_command_buffer);
}

void GraphicsEngine::EndGuiUpdate()
{
    const auto gui_command_buffer = guiCommandPool->vkCommandBuffers[currentFrame];

    for (auto &single_gui_command : singleGuiCommands[m_currentRenderPass]) {
        single_gui_command(gui_command_buffer);
    }
    singleGuiCommands.at(m_currentRenderPass).clear();

    for (auto &repeated_gui_command : repeatedGuiCommands) {
        repeated_gui_command(gui_command_buffer, *m_currentRenderPass);
    }

    windowRenderPass->EndRenderPass(gui_command_buffer);
    guiCommandPool->EndCommandBuffer(gui_command_buffer);
}

void GraphicsEngine::Render()
{
    vkResetFences(device, 1, &m_inFlightFences[currentFrame]);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const std::array command_buffers = {drawCommandPool->vkCommandBuffers[currentFrame],
                                        guiCommandPool->vkCommandBuffers[currentFrame]};

    const VkSemaphore wait_semaphores[] = {m_imageAvailableSemaphores[currentFrame]};
    constexpr VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
    submit_info.pCommandBuffers = command_buffers.data();
    const VkSemaphore signal_semaphores[] = {m_renderFinishedSemaphores[currentFrame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    VK_ASSERT(vkQueueSubmit(device.graphicsQueue, 1, &submit_info, m_inFlightFences[currentFrame]),
              "Failed to submit draw command buffer!");

    /*
    drawCommandPool->ClearSingleUseCommands();
    guiCommandPool->ClearSingleUseCommands();
    memoryCommandPool->ClearSingleUseCommands();
    */

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    const VkSwapchainKHR present_swap_chains[] = {*m_currentSurface->swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = present_swap_chains;
    present_info.pImageIndices = &m_currentImageIndex;
    present_info.pResults = nullptr;  // Optional

    const auto result = vkQueuePresentKHR(device.presentQueue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // This should be handled by even OnWindowResize
        // current_surface->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % Engine::MAX_FRAMES_IN_FLIGHT;
}

// Dont call while rendering
void GraphicsEngine::OnViewportResize(uint32_t Width, uint32_t Height)
{
    m_graphicsInstance->RecreateViewportSwapChain(Width, Height);
}

void GraphicsEngine::OnWindowResized(Window *Window, int Width, int Height)
{
    Window->GetSurface().RecreateSwapChain(Width, Height);
}
}  // namespace Slipper