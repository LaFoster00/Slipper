
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

GraphicsEngine *GraphicsEngine::instance = nullptr;
bool GraphicsEngine::bSetupDefaultAssets = true;

GraphicsEngine::GraphicsEngine() : DeviceDependentObject()
{
    if (instance != nullptr)
        return;

    instance = this;

    renderCommandPool = new CommandPool(device.graphicsQueue,
                                        device.queueFamilyIndices.graphicsFamily.value(),
                                        Engine::MAX_FRAMES_IN_FLIGHT);

    memoryCommandPool = new CommandPool(
        device.transferQueue ? device.transferQueue : device.graphicsQueue,
        device.transferQueue ? device.queueFamilyIndices.transferFamily.value() :
                               device.queueFamilyIndices.graphicsFamily.value());

    if (bSetupDefaultAssets) {
        SetupDefaultAssets();
        CreateSyncObjects();
    }
}

void GraphicsEngine::SetSetupDefaultAssets(const bool Value)
{
    bSetupDefaultAssets = Value;
}

GraphicsEngine::~GraphicsEngine()
{
    renderPasses.clear();

    delete memoryCommandPool;
    delete renderCommandPool;

    for (const auto image_available_semaphore : m_imageAvailableSemaphores) {
        vkDestroySemaphore(device.logicalDevice, image_available_semaphore, nullptr);
    }

    for (const auto render_finished_semaphore : m_renderFinishedSemaphores) {
        vkDestroySemaphore(device.logicalDevice, render_finished_semaphore, nullptr);
    }

    for (const auto in_flight_fence : m_inFlightFences) {
        vkDestroyFence(device.logicalDevice, in_flight_fence, nullptr);
    }
    
    shaders.clear();

    models.clear();
    textures.clear();
}

void GraphicsEngine::SetupDefaultAssets()
{
    // The default shader depends on these so initialize them first
    textures.emplace_back(Texture2D::LoadTexture(DEMO_TEXTURE_PATH, true));
    models.emplace_back(std::make_unique<Model>(DEMO_MODEL_PATH));

    /* Create shader for this pipeline. */
    std::vector<std::tuple<std::string_view, ShaderType>> shader_stages = {
        {"./EngineContent/Shaders/Spir-V/Basic.vert.spv", ShaderType::VERTEX},
        {"./EngineContent/Shaders/Spir-V/Basic.frag.spv", ShaderType::FRAGMENT}};

    shaders.emplace_back(std::make_unique<Shader>("BasicVertex",
                                                  shader_stages));
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
            vkCreateSemaphore(
                device.logicalDevice, &semaphore_info, nullptr, &m_imageAvailableSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(
            vkCreateSemaphore(
                device.logicalDevice, &semaphore_info, nullptr, &m_renderFinishedSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(vkCreateFence(device.logicalDevice, &fence_info, nullptr, &m_inFlightFences[i]),
                  "Failed to create fence!")
    }
}

RenderPass *GraphicsEngine::CreateRenderPass(const VkFormat AttachmentFormat,
                                             const VkFormat DepthFormat)
{
    return renderPasses.emplace_back(std::make_unique<RenderPass>(AttachmentFormat, DepthFormat)).get();
}

void GraphicsEngine::SetupDebugRender(Surface &Surface)
{
    surfaces.insert(&Surface);

    const auto render_pass = CreateRenderPass(Surface.swapChain->GetImageFormat(), Surface.swapChain->GetDepthFormat());

    Surface.RegisterRenderPass(*render_pass);
    shaders[0]->RegisterForRenderPass(render_pass, Surface.GetResolution());

    SetupSimpleDraw();
}

void GraphicsEngine::SetupSimpleDraw()
{
    AddRepeatedDrawCommand([=, this](const VkCommandBuffer &CommandBuffer,
                                     const RenderPass &RenderPass) {
        const auto debug_shader = *RenderPass.registeredShaders.begin();
        debug_shader->Bind(CommandBuffer, &RenderPass);

        static auto start_time = std::chrono::high_resolution_clock::now();

        const auto current_time = std::chrono::high_resolution_clock::now();
        const float time = std::chrono::duration<float, std::chrono::seconds::period>(
                               current_time - start_time)
                               .count();

        UniformMVP mvp;
        mvp.model = glm::rotate(
            glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

void GraphicsEngine::AddRepeatedDrawCommand(
    const std::function<void(const VkCommandBuffer &, const RenderPass &)> Command)
{
    repeatedRenderCommands.push_back(Command);
}

void GraphicsEngine::DrawFrame()
{
    vkWaitForFences(device.logicalDevice, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    Surface &current_surface = **surfaces.begin();
    RenderPass &current_render_pass = *current_surface.renderPasses[0];
    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device.logicalDevice,
                                            *current_surface.swapChain,
                                            UINT64_MAX,
                                            m_imageAvailableSemaphores[currentFrame],
                                            VK_NULL_HANDLE,
                                            &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        current_surface.RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(device.logicalDevice, 1, &m_inFlightFences[currentFrame]);

    const VkCommandBuffer command_buffer = renderCommandPool->vkCommandBuffers[currentFrame];
    renderCommandPool->BeginCommandBuffer(command_buffer);

    for (const auto render_pass : current_surface.renderPasses) {
        current_render_pass.BeginRenderPass(
            current_surface.swapChain.get(), image_index, command_buffer);

        for (auto &command : repeatedRenderCommands) {
            command(command_buffer, *render_pass);
        }

        current_render_pass.EndRenderPass(command_buffer);
    }
    renderCommandPool->EndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkSemaphore wait_semaphores[] = {m_imageAvailableSemaphores[currentFrame]};
    constexpr VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
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

    const VkSwapchainKHR present_swap_chains[] = {*current_surface.swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = present_swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;  // Optional

    result = vkQueuePresentKHR(device.presentQueue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_framebufferResized) {
        m_framebufferResized = false;
        current_surface.RecreateSwapChain();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % Engine::MAX_FRAMES_IN_FLIGHT;
}

void GraphicsEngine::OnWindowResized()
{
    m_framebufferResized = true;
}