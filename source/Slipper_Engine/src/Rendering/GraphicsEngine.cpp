
#include "GraphicsEngine.h"

#include "Drawing/CommandPool.h"
#include "GraphicsPipeline/RenderPass.h"
#include "Mesh/Mesh.h"
#include "Mesh/UniformBuffer.h"
#include "Presentation/Surface.h"
#include "Setup/Device.h"
#include "Texture/Texture2D.h"
#include "common_defines.h"

#include "glm/gtc/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>

#include "Model/Model.h"
#include "Shader/Shader.h"
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
    renderPasses.clear();

    delete memoryCommandPool;
    delete renderCommandPool;

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

    m_graphicsInstance->renderCommandPool = new CommandPool(
        device.graphicsQueue,
        device.queueFamilyIndices.graphicsFamily.value(),
        Engine::MAX_FRAMES_IN_FLIGHT);

    m_graphicsInstance->memoryCommandPool = new CommandPool(
        device.transferQueue ? device.transferQueue : device.graphicsQueue,
        device.transferQueue ? device.queueFamilyIndices.transferFamily.value() :
                               device.queueFamilyIndices.graphicsFamily.value());

    m_graphicsInstance->mainRenderPass = m_graphicsInstance->CreateRenderPass(
        "Main", SwapChain::swapChainFormat, Texture2D::FindDepthFormat());

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
                                             const VkFormat AttachmentFormat,
                                             const VkFormat DepthFormat)
{
    renderPasses[Name] = std::make_unique<RenderPass>(Name, AttachmentFormat, DepthFormat);
    return renderPasses[Name].get();
}

void GraphicsEngine::AddWindow(Window &Window)
{
    windows.insert(&Window);
    Window.GetSurface().CreateSwapChain();
    Window.GetSurface().RegisterRenderPass(*mainRenderPass);
}

void GraphicsEngine::SetupDebugRender(Surface &Surface)
{
    shaders[0]->RegisterForRenderPass(mainRenderPass, Surface.GetResolution());

    SetupSimpleDraw();
}

void GraphicsEngine::SetupSimpleDraw()
{
    SubmitRepeatedDrawCommand([=, this](const VkCommandBuffer &CommandBuffer,
                                        const RenderPass &RenderPass) {
        const auto debug_shader = *RenderPass.registeredShaders.begin();
        debug_shader->Bind(CommandBuffer, &RenderPass);

        UniformMVP mvp;
        mvp.model = glm::rotate(glm::mat4(1.0f),
                                Time::TimeSinceStartup() * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.projection = glm::perspective(
            glm::radians(45.0f),
            RenderPass.GetActiveSwapChain()->GetResolution().width /
                (float)RenderPass.GetActiveSwapChain()->GetResolution().height,
            0.1f,
            10.0f);

        mvp.projection[1][1] *= -1;

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
    singleDrawCommand[RenderPass].push_back(Command);
}

void GraphicsEngine::SubmitRepeatedDrawCommand(
    const std::function<void(const VkCommandBuffer &, const RenderPass &)> Command)
{
    repeatedDrawCommands.push_back(Command);
}

void GraphicsEngine::BeginFrame()
{
    vkWaitForFences(device, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    m_currentSurface = &(*windows.begin())->GetSurface();
    const auto result = vkAcquireNextImageKHR(device,
                                              *m_currentSurface->swapChain,
                                              UINT64_MAX,
                                              m_imageAvailableSemaphores[currentFrame],
                                              VK_NULL_HANDLE,
                                              &m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // This should be handled by even OnWindowResize
        // current_surface->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &m_inFlightFences[currentFrame]);

    m_currentCommandBuffer = renderCommandPool->vkCommandBuffers[currentFrame];
    renderCommandPool->BeginCommandBuffer(m_currentCommandBuffer);
}

void GraphicsEngine::EndUpdate()
{
    for (const auto render_pass : m_currentSurface->renderPasses) {
        if (render_pass != mainRenderPass)
            continue;
        render_pass->BeginRenderPass(
            m_currentSurface->swapChain.get(), m_currentImageIndex, m_currentCommandBuffer);

        for (auto &command : singleDrawCommand[render_pass]) {
            command(m_currentCommandBuffer);
        }
        singleDrawCommand.at(render_pass).clear();

        for (auto &command : repeatedDrawCommands) {
            command(m_currentCommandBuffer, *render_pass);
        }

        render_pass->EndRenderPass(m_currentCommandBuffer);
    }

    m_currentRenderPass = mainRenderPass;
    m_currentRenderPass->BeginRenderPass(
        m_currentSurface->swapChain.get(), m_currentImageIndex, m_currentCommandBuffer);

    for (auto &command : singleDrawCommand[m_currentRenderPass]) {
        command(m_currentCommandBuffer);
    }
    singleDrawCommand.at(m_currentRenderPass).clear();

    for (auto &command : repeatedDrawCommands) {
        command(m_currentCommandBuffer, *m_currentRenderPass);
    }
}

void GraphicsEngine::EndFrame()
{
    mainRenderPass->EndRenderPass(m_currentCommandBuffer);
    renderCommandPool->EndCommandBuffer(m_currentCommandBuffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkSemaphore wait_semaphores[] = {m_imageAvailableSemaphores[currentFrame]};
    constexpr VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_currentCommandBuffer;
    const VkSemaphore signal_semaphores[] = {m_renderFinishedSemaphores[currentFrame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    VK_ASSERT(vkQueueSubmit(device.graphicsQueue, 1, &submit_info, m_inFlightFences[currentFrame]),
              "Failed to submit draw command buffer!");

    renderCommandPool->ClearSingleUseCommands();
    memoryCommandPool->ClearSingleUseCommands();

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

void GraphicsEngine::OnWindowResized(Window *Window, int Width, int Height)
{
    Window->GetSurface().RecreateSwapChain(Width, Height);
}
}  // namespace Slipper