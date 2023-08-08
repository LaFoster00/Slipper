
#include "GraphicsEngine.h"

#include "Camera.h"
#include "CameraComponent.h"
#include "Core/Application.h"
#include "Core/ModelManager.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Material.h"
#include "MaterialManager.h"
#include "Mesh/Mesh.h"
#include "Model/Model.h"
#include "Presentation/OffscreenSwapChain.h"
#include "Presentation/Surface.h"
#include "RenderPass.h"
#include "RendererComponent.h"
#include "Texture/Texture2D.h"
#include "Window.h"

namespace Slipper
{
GraphicsEngine *GraphicsEngine::m_graphicsInstance = nullptr;

GraphicsEngine::GraphicsEngine()
{
    ASSERT(!m_graphicsInstance, "Graphics Engine allready created!");
    m_graphicsInstance = this;
}

GraphicsEngine::~GraphicsEngine()
{
    renderingStages.clear();

    viewportSwapChain.reset();

    renderPassNames.clear();
    renderPasses.clear();

    memoryCommandPool.reset();

    for (const auto in_flight_fence : m_renderingInFlightFences) {
        device.logicalDevice.destroyFence(in_flight_fence);
    }

    for (const auto compute_in_flight_fence : m_computeInFlightFences) {
        device.logicalDevice.destroyFence(compute_in_flight_fence);
    }

    ShaderManager::Shutdown();
    ModelManager::Shutdown();
    TextureManager::Shutdown();
}

void GraphicsEngine::Init()
{
    m_graphicsInstance = new GraphicsEngine();
    auto &device = Device::Get();

    vk::FenceCreateInfo fence_info{};
    fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

    m_graphicsInstance->m_renderingInFlightFences.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    m_graphicsInstance->m_computeInFlightFences.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
        VK_HPP_ASSERT(device.logicalDevice.createFence(
                          &fence_info, nullptr, &m_graphicsInstance->m_computeInFlightFences[i]),
                      "Failed to create compute fence !")

        VK_HPP_ASSERT(device.logicalDevice.createFence(
                          &fence_info, nullptr, &m_graphicsInstance->m_renderingInFlightFences[i]),
                      "Failed to create graphics fence !")
    }

    m_graphicsInstance->memoryCommandPool = std::make_unique<CommandPool>(
        device.transferQueue ? device.transferQueue : device.graphicsQueue,
        device.transferQueue ? device.queueFamilyIndices.transferFamily.value() :
                               device.queueFamilyIndices.graphicsFamily.value());

    m_graphicsInstance->windowRenderPass = m_graphicsInstance->CreateRenderPass(
        "Window", SwapChain::swapChainFormat, Texture2D::FindDepthFormat(), true);

    m_graphicsInstance->viewportRenderPass = m_graphicsInstance->CreateRenderPass(
        "Viewport", Engine::TARGET_VIEWPORT_COLOR_FORMAT, Texture2D::FindDepthFormat(), false);

    m_graphicsInstance->viewportSwapChain = new OffscreenSwapChain(
        Application::Get().window->GetSize(),
        Engine::TARGET_VIEWPORT_COLOR_FORMAT,
        Engine::MAX_FRAMES_IN_FLIGHT,
        true);

    m_graphicsInstance->viewportRenderingStage = m_graphicsInstance->AddRenderingStage(
        "Viewport",
        m_graphicsInstance->viewportSwapChain,
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
    // The default material depends on these so initialize them first
    TextureManager::Load2D(DEMO_TEXTURE_PATH, true);
    ModelManager::Load(DEMO_MODEL_PATH);

    /* CreateCamera material for this pipeline. */

    MaterialManager::AddMaterial(
        "Basic",
        ShaderManager::LoadGraphicsShader({{"./EngineContent/Shaders/Spir-V/Basic.vert.spv"},
                                           {"./EngineContent/Shaders/Spir-V/Basic.frag.spv"}}))
        ->SetUniform("texSampler", *TextureManager::Get2D("viking_room"));
}

RenderPass *GraphicsEngine::CreateRenderPass(const std::string &Name,
                                             const vk::Format RenderingFormat,
                                             const vk::Format DepthFormat,
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

Entity GraphicsEngine::GetDefaultCamera()
{
    for (const auto entity : EcsInterface::GetRegistry().view<Camera>()) {
        return entity;
    }
    return CreateCamera("Default Camera");
}

void GraphicsEngine::AddWindow(Window &Window)
{
    windows.push_back(&Window);
    Window.GetSurface().CreateSwapChain();

    windowRenderingStage = AddRenderingStage("Window",
                                             Window.GetSurface().swapChain.get(),
                                             device.graphicsQueue,
                                             device.queueFamilyIndices.graphicsFamily.value(),
                                             true);

    windowRenderingStage->RegisterForRenderPass(windowRenderPass);
}

NonOwningPtr<RenderingStage> GraphicsEngine::AddRenderingStage(std::string Name,
                                                               NonOwningPtr<SwapChain> SwapChain,
                                                               VkQueue CommandQueue,
                                                               uint32_t CommandQueueFamilyIndex,
                                                               bool NativeSwapChain,
                                                               int32_t CommandBufferCount)
{

    if (renderingStages.contains(Name)) {
        LOG_FORMAT("Rendering stage '{}' does already exist. Returned nullptr", Name);
        return nullptr;
    }

    return renderingStages
        .emplace(Name,
                 new RenderingStage(Name,
                                    SwapChain,
                                    CommandQueue,
                                    CommandQueueFamilyIndex,
                                    NativeSwapChain,
                                    CommandBufferCount))
        .first->second.get();
}

void GraphicsEngine::SetupDebugRender(Surface &Surface) const
{
    ShaderManager::TryGetGraphicsShader("Basic")->RegisterRenderPass(viewportRenderPass);
    SetupSimpleDraw();
}

void GraphicsEngine::SetupSimpleDraw() const
{
    Entity debug_model = SceneObject::Create("Debug Model");
    debug_model.AddComponent<Renderer>(viewportRenderingStage,
                                       ModelManager::GetModel("viking_room"),
                                       MaterialManager::GetMaterial("Basic"));
}

void GraphicsEngine::NewFrame() const
{
    device.logicalDevice.waitForFences(
        {m_computeInFlightFences[m_currentFrame], m_renderingInFlightFences[m_currentFrame]},
        VK_TRUE,
        UINT64_MAX);
}

void GraphicsEngine::BeginRenderingStage(std::string_view Name)
{
    if (m_currentRenderingStage) {
        LOG_FORMAT("Already rendering stage '{}'. End it first!", m_currentRenderingStage->name);
        return;
    }

    if (renderingStages.contains(static_cast<std::string>(Name))) {
        m_currentRenderingStage = renderingStages.at(static_cast<std::string>(Name));
        m_currentRenderingStage->BeginRender();
        return;
    }

    LOG_FORMAT("Rendering stage '{}' was not found.", Name);
}

void GraphicsEngine::EndRenderingStage()
{
    if (m_currentRenderingStage) {
        m_currentRenderingStage->EndRender();
        m_currentRenderingStage = nullptr;
    }
}

void GraphicsEngine::EndFrame()
{
    // We reset both fences together since we also wait for both together so there isnt really a point in doing them seperatly
    device.logicalDevice.resetFences(
        {m_renderingInFlightFences[m_currentFrame], m_computeInFlightFences[m_currentFrame]});

    vk::SubmitInfo submit_info;

    std::vector<vk::CommandBuffer> command_buffers;
    command_buffers.reserve(renderingStages.size());

    for (const auto &rendering_stage : renderingStages | std::ranges::views::values) {
        command_buffers.push_back(rendering_stage->commandPool->vkCommandBuffers[m_currentFrame]);
    }

    std::vector<vk::Semaphore> rendering_finished_semaphores;
    for (const auto &rendering_stage : renderingStages | std::ranges::views::values) {
        if (rendering_stage->IsPresentStage()) {
            rendering_finished_semaphores.emplace_back(
                rendering_stage->GetCurrentImageAvailableSemaphore());
        }
    }

    constexpr vk::PipelineStageFlags wait_stages[] = {
        vk::PipelineStageFlagBits::eVertexInput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submit_info.setWaitSemaphores(rendering_finished_semaphores);
    submit_info.setWaitDstStageMask(wait_stages);
    submit_info.setCommandBuffers(command_buffers);

    std::vector<vk::Semaphore> signal_semaphores;
    for (const auto &rendering_stage : renderingStages | std::ranges::views::values) {
        if (rendering_stage->IsPresentStage()) {
            signal_semaphores.emplace_back(rendering_stage->GetCurrentRenderFinishSemaphore());
        }
    }
    submit_info.setSignalSemaphores(signal_semaphores);

    VK_HPP_ASSERT(
        device.graphicsQueue.submit(1, &submit_info, m_renderingInFlightFences[m_currentFrame]),
        "Failed to submit draw command buffer!")

    vk::PresentInfoKHR present_info;
    present_info.setWaitSemaphores(signal_semaphores);

    std::vector<vk::SwapchainKHR> present_swap_chains;
    std::vector<uint32_t> swap_chain_image_indices;

    for (const auto &rendering_stage : renderingStages | std::ranges::views::values) {
        if (rendering_stage->IsPresentStage()) {
            present_swap_chains.push_back(*rendering_stage->GetSwapChain());
            swap_chain_image_indices.push_back(rendering_stage->GetCurrentImageIndex());
        }
    }
    present_info.setSwapchains(present_swap_chains);
    present_info.setImageIndices(swap_chain_image_indices);

    present_info.pResults = nullptr;  // Optional

    const auto result = device.presentQueue.presentKHR(&present_info);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        auto capabilities = device.physicalDevice.getSurfaceCapabilitiesKHR(
            windows[0]->GetSurface());
        OnWindowResized(
            windows[0], capabilities.currentExtent.width, capabilities.currentExtent.height);
    }
    else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % Engine::MAX_FRAMES_IN_FLIGHT;
}

// Dont call while rendering
void GraphicsEngine::OnViewportResize(NonOwningPtr<RenderingStage> Stage,
                                      uint32_t Width,
                                      uint32_t Height)
{
    Stage->ChangeResolution(Width, Height);
}

void GraphicsEngine::OnWindowResized(Window *Window, int Width, int Height)
{
    m_graphicsInstance->windowRenderingStage->ChangeResolution(Width, Height);
}
}  // namespace Slipper