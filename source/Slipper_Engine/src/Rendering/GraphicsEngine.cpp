
#include "GraphicsEngine.h"

#include "Drawing/CommandPool.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Mesh/Mesh.h"
#include "Mesh/UniformBuffer.h"
#include "Presentation/Surface.h"

#include "Camera.h"
#include "Core/Application.h"
#include "Core/ModelManager.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Drawing/Sampler.h"
#include "Model/Model.h"
#include "Presentation/OffscreenSwapChain.h"
#include "RenderingStage.h"
#include "Shader/Shader.h"
#include "Texture/Texture2D.h"
#include "Time/Time.h"
#include "TransformComponent.h"
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
    viewportRenderingStage.reset();
    windowRenderingStage.reset();

    renderPassNames.clear();
    renderPasses.clear();

    memoryCommandPool.reset();

    for (const auto in_flight_fence : m_inFlightFences) {
        vkDestroyFence(device, in_flight_fence, nullptr);
    }

    ShaderManager::Shutdown();
    ModelManager::Shutdown();
    TextureManager::Shutdown();
}

void GraphicsEngine::Init()
{
    m_graphicsInstance = new GraphicsEngine();
    auto &device = Device::Get();

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_graphicsInstance->m_inFlightFences.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
        VK_ASSERT(
            vkCreateFence(device, &fence_info, nullptr, &m_graphicsInstance->m_inFlightFences[i]),
            "Failed to create fence!")
    }

    m_graphicsInstance->memoryCommandPool = std::make_unique<CommandPool>(
        device.transferQueue ? device.transferQueue : device.graphicsQueue,
        device.transferQueue ? device.queueFamilyIndices.transferFamily.value() :
                               device.queueFamilyIndices.graphicsFamily.value());

    m_graphicsInstance->windowRenderPass = m_graphicsInstance->CreateRenderPass(
        "Window", SwapChain::swapChainFormat, Texture2D::FindDepthFormat(), true);

    m_graphicsInstance->viewportRenderPass = m_graphicsInstance->CreateRenderPass(
        "Viewport", Engine::TARGET_VIEWPORT_COLOR_FORMAT, Texture2D::FindDepthFormat(), false);

    auto viewport_swap_chain = new OffscreenSwapChain(Application::Get().window->GetSize(),
                                                      Engine::TARGET_VIEWPORT_COLOR_FORMAT,
                                                      Engine::MAX_FRAMES_IN_FLIGHT,
                                                      true);

    m_graphicsInstance->viewportRenderingStage = new RenderingStage(
        "Viewport",
        viewport_swap_chain,
        device.graphicsQueue,
        device.queueFamilyIndices.graphicsFamily.value(),
        false);

    m_graphicsInstance->viewportRenderingStage->RegisterForRenderPass(
        m_graphicsInstance->viewportRenderPass);

    SetupDebugResources();
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
    TextureManager::Load2D(DEMO_TEXTURE_PATH, true);
    ModelManager::Load(DEMO_MODEL_PATH);

    /* Create shader for this pipeline. */
    ShaderManager::LoadShader({{"./EngineContent/Shaders/Spir-V/Basic.vert.spv"},
                               {"./EngineContent/Shaders/Spir-V/Basic.frag.spv"}})
        ->BindShaderParameter("texSampler", TextureManager::Get2D("viking_room"));
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

Entity &GraphicsEngine::GetDefaultCamera()
{
    static bool init = false;
    if (!init) {
        m_defaultCamera = Camera::Create();
        init = true;
    }

    return m_defaultCamera;
}

void GraphicsEngine::AddWindow(Window &Window)
{
    windows.insert(&Window);
    Window.GetSurface().CreateSwapChain();

    m_graphicsInstance->windowRenderingStage = new RenderingStage(
        "Window",
        Window.GetSurface().swapChain.get(),
        device.graphicsQueue,
        device.queueFamilyIndices.graphicsFamily.value(),
        true);

    m_graphicsInstance->windowRenderingStage->RegisterForRenderPass(windowRenderPass);
}

void GraphicsEngine::SetupDebugRender(Surface &Surface)
{
    auto debug_shader = ShaderManager::GetShader("Basic");
    debug_shader->RegisterForRenderPass(viewportRenderPass, Surface.GetResolution());

    SetupSimpleDraw();
}

void GraphicsEngine::SetupSimpleDraw()
{
    viewportRenderingStage->SubmitRepeatedDrawCommand(
        viewportRenderPass,
        [=, this](const VkCommandBuffer &CommandBuffer, const RenderPass &RenderPass) {
            const auto debug_shader = *RenderPass.registeredShaders.begin();
            debug_shader->Use(CommandBuffer, &RenderPass);

            auto &camera = GetDefaultCamera();
            auto &cam_parameters = camera.GetComponent<Camera::Parameters>();
            auto &cam_transform = camera.GetComponent<Transform>();
            cam_parameters.UpdateViewTransform(cam_transform);
            Transform transform({0, 0, 0}, {1, 1, 1}, {0, 0, Time::TimeSinceStartup() * 90.0f});

            UniformVP vp;
            vp.view = cam_parameters.view;
            vp.projection = cam_parameters.GetProjection(
                RenderPass.GetActiveSwapChain()->GetResolution().width /
                static_cast<float>(RenderPass.GetActiveSwapChain()->GetResolution().height));

            UniformModel model;
            model.model = transform.GetModelMatrix();

            debug_shader->GetUniformBuffer("vp")->SubmitData(&vp);
            debug_shader->GetUniformBuffer("m")->SubmitData(&model);

            ModelManager::GetModel("viking_room")->Draw(CommandBuffer);
        });
}

void GraphicsEngine::BeginUpdate()
{
    vkWaitForFences(device,
                    1,
                    &m_inFlightFences[GraphicsEngine::Get().GetCurrentFrame()],
                    VK_TRUE,
                    UINT64_MAX);

    m_currentSurface = &(*windows.begin())->GetSurface();
    viewportRenderingStage->BeginRender();
}

void GraphicsEngine::EndUpdate() const
{
    viewportRenderingStage->EndRender();
}

void GraphicsEngine::BeginGuiUpdate() const
{
    windowRenderingStage->BeginRender();
}

void GraphicsEngine::EndGuiUpdate() const
{
    windowRenderingStage->EndRender();
}

void GraphicsEngine::Render()
{
    vkResetFences(device, 1, &m_inFlightFences[GetCurrentFrame()]);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const std::array command_buffers = {
        viewportRenderingStage->commandPool->vkCommandBuffers[m_currentFrame],
        windowRenderingStage->commandPool->vkCommandBuffers[m_currentFrame]};

    const VkSemaphore wait_semaphores[] = {
        windowRenderingStage->GetCurrentImageAvailableSemaphore()};
    constexpr VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
    submit_info.pCommandBuffers = command_buffers.data();
    const VkSemaphore signal_semaphores[] = {
        windowRenderingStage->GetCurrentRenderFinishSemaphore()};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    VK_ASSERT(
        vkQueueSubmit(device.graphicsQueue, 1, &submit_info, m_inFlightFences[m_currentFrame]),
        "Failed to submit draw command buffer!");

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    const std::vector<VkSwapchainKHR> present_swap_chains = {
        windowRenderingStage->GetSwapChain()->vkSwapChain};
    const std::vector<uint32_t> swap_chain_image_indices = {
        windowRenderingStage->GetCurrentImageIndex()};
    present_info.swapchainCount = static_cast<uint32_t>(present_swap_chains.size());
    present_info.pSwapchains = present_swap_chains.data();
    present_info.pImageIndices = swap_chain_image_indices.data();
    ;
    present_info.pResults = nullptr;  // Optional

    const auto result = vkQueuePresentKHR(device.presentQueue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // This should be handled by even OnWindowResize
        // current_surface->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % Engine::MAX_FRAMES_IN_FLIGHT;
}

// Dont call while rendering
void GraphicsEngine::OnViewportResize(uint32_t Width, uint32_t Height)
{
    m_graphicsInstance->viewportRenderingStage->ChangeResolution(Width, Height);
}

void GraphicsEngine::OnWindowResized(Window *Window, int Width, int Height)
{
    m_graphicsInstance->windowRenderingStage->ChangeResolution(Width, Height);
}
}  // namespace Slipper