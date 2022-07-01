
#include "GraphicsEngine.h"

#include <algorithm>
#include <filesystem>

#include "Mesh/Mesh.h"
#include "Mesh/UniformBuffer.h"
#include "Presentation/Surface.h"
#include "Setup/Device.h"
#include "Window/Window.h"
#include "common_defines.h"
#include "glm/gtc/matrix_transform.hpp"

GraphicsEngine *GraphicsEngine::instance = nullptr;

GraphicsEngine::GraphicsEngine(Device &device, bool setupDefaultAssets) : device(device)
{
    if (instance != nullptr)
        return;

    instance = this;

    renderCommandPool = new CommandPool(device, Engine::MaxFramesInFlight);
    memoryCommandPool = new CommandPool(device, 0);

    if (setupDefaultAssets) {
        SetupDefaultAssets();
        CreateSyncObjects();
    }
}

GraphicsEngine::~GraphicsEngine()
{
    meshes.clear();

    renderPasses.clear();
    swapChains.clear();

    delete memoryCommandPool;
    delete renderCommandPool;

    for (const auto m_imageAvailableSemaphore : m_imageAvailableSemaphores) {
        vkDestroySemaphore(device.logicalDevice, m_imageAvailableSemaphore, nullptr);
    }

    for (const auto m_renderFinishedSemaphore : m_renderFinishedSemaphores) {
        vkDestroySemaphore(device.logicalDevice, m_renderFinishedSemaphore, nullptr);
    }

    for (const auto m_inFlightFence : m_inFlightFences) {
        vkDestroyFence(device.logicalDevice, m_inFlightFence, nullptr);
    }

    // TODO move this into shader once the descriptor set system is more fleshed out
    vkDestroyDescriptorSetLayout(device, UniformMVP().GetDescriptorSetLayout(), nullptr);
    shaders.clear();
}

SwapChain *GraphicsEngine::CreateSwapChain(Window &window, Surface &surface)
{
    // Create new swap chain for this device and insert it into the mapping
    const auto newSwapDependency = swapChainDependencies.insert(std::make_pair(
        swapChains.emplace_back(std::make_unique<SwapChain>(window, surface, true)).get(),
        std::tuple<Window &, Surface &, std::vector<RenderPass *>>(window, surface, {})));
    return newSwapDependency.first->first;
}

void GraphicsEngine::CleanupSwapChain(SwapChain *SwapChain)
{
    for (auto &renderPass : renderPasses) {
        renderPass->DestroySwapChainFramebuffers(SwapChain);
    }

    for (auto swapChainIt = swapChains.begin(); swapChainIt != swapChains.end(); ++swapChainIt) {
        if (swapChainIt->get() == SwapChain) {
            swapChains.erase(swapChainIt);
            break;
        }
    }

    swapChainDependencies.erase(SwapChain);
}

void GraphicsEngine::RecreateSwapChain(SwapChain *SwapChain)
{
    int width = 0, height = 0;
    auto [windowDep, surfaceDep, renderPassesDep] = swapChainDependencies.at(SwapChain);
    glfwGetFramebufferSize(windowDep.glfwWindow, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(windowDep.glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.logicalDevice);

    CleanupSwapChain(SwapChain);

    auto newSwapChain = CreateSwapChain(windowDep, surfaceDep);

    for (const auto renderPass : renderPassesDep) {
        /* Create frambuffers for this swap chain */
        renderPass->CreateSwapChainFramebuffers(newSwapChain);
        std::get<2>(swapChainDependencies.at(newSwapChain)).push_back(renderPass);

        for (const auto renderPassShader : renderPass->registeredShaders){
            renderPassShader->ChangeResolutionForRenderPass(renderPass,
                                                            newSwapChain->GetResolution());
        }
    }
}

void GraphicsEngine::SetupDefaultAssets()
{
    /* Create shader for this pipeline. */
    std::vector<std::tuple<std::string_view, ShaderType>> shaderStages = {
        {"./EngineContent/Shaders/Spir-V/Basic.vert.spv", ShaderType::Vertex},
        {"./EngineContent/Shaders/Spir-V/Basic.frag.spv", ShaderType::Fragment}};

    shaders.emplace_back(std::make_unique<Shader>("BasicVertex",
                                                  shaderStages,
                                                  Engine::MaxFramesInFlight,
                                                  UniformMVP().GetDataSize(),
                                                  UniformMVP().GetDescriptorSetLayout()));

    meshes.emplace_back(std::make_unique<Mesh>(*memoryCommandPool,
                                               DebugTriangleVertices.data(),
                                               DebugTriangleVertices.size(),
                                               DebugTriangleIndices.data(),
                                               DebugTriangleIndices.size()));
}

void GraphicsEngine::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_imageAvailableSemaphores.resize(Engine::MaxFramesInFlight);
    m_renderFinishedSemaphores.resize(Engine::MaxFramesInFlight);
    m_inFlightFences.resize(Engine::MaxFramesInFlight);

    for (int i = 0; i < Engine::MaxFramesInFlight; ++i) {
        VK_ASSERT(
            vkCreateSemaphore(
                device.logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(
            vkCreateSemaphore(
                device.logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]),
            "Failed to create semaphore!")
        VK_ASSERT(vkCreateFence(device.logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]),
                  "Failed to create fence!")
    }
}

RenderPass *GraphicsEngine::CreateRenderPass(const VkFormat attachmentFormat)
{
    return renderPasses.emplace_back(std::make_unique<RenderPass>(device, attachmentFormat)).get();
}

Shader *GraphicsEngine::SetupDebugRender(Window &window, Surface &surface)
{
    const auto SwapChain = CreateSwapChain(window, surface);
    const auto renderPass = CreateRenderPass(SwapChain->GetFormat());

    /* Create renderpass for this pipeline */
    renderPass->CreateSwapChainFramebuffers(SwapChain);
    std::get<2>(swapChainDependencies.at(SwapChain)).push_back(renderPass);
    shaders[0]->RegisterForRenderPass(renderPass, SwapChain->GetResolution());
    return shaders[0].get();
}

void GraphicsEngine::SetupSimpleDraw()
{
    AddRepeatedDrawCommand([=, this](const VkCommandBuffer &commandBuffer,
                                     const SwapChain &swapChain) {
        meshes[0]->Bind(commandBuffer);

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                                startTime)
                         .count();

        UniformMVP mvp{};
        mvp.model = glm::rotate(
            glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.projection = glm::perspective(glm::radians(45.0f),
                                          swapChain.GetResolution().width /
                                              (float)swapChain.GetResolution().height,
                                          0.1f,
                                          10.0f);

        mvp.projection[1][1] *= -1;

        shaders[0]->GetUniformBuffer(currentFrame).SubmitData(&mvp);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshes[0]->NumIndex()), 1, 0, 0, 0);
    });
}

void GraphicsEngine::AddRepeatedDrawCommand(
    std::function<void(const VkCommandBuffer &, const SwapChain &)> command)
{
    repeatedRenderCommands.push_back(command);
}

void GraphicsEngine::DrawFrame()
{
    vkWaitForFences(device.logicalDevice, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device.logicalDevice,
                                            swapChains[0]->vkSwapChain,
                                            UINT64_MAX,
                                            m_imageAvailableSemaphores[currentFrame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_framebufferResized) {
        RecreateSwapChain(swapChains[0].get());
        m_framebufferResized = false;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(device.logicalDevice, 1, &m_inFlightFences[currentFrame]);

    VkCommandBuffer commandBuffer = renderCommandPool->BeginCommandBuffer(currentFrame);
    renderPasses[0]->BeginRenderPass(swapChains[0].get(), imageIndex, commandBuffer);

    shaders[0]->Bind(commandBuffer, renderPasses[0].get(), currentFrame);

    for (auto &command : repeatedRenderCommands) {
        command(commandBuffer, *swapChains[0]);
    }

    renderPasses[0]->EndRenderPass(commandBuffer);
    renderCommandPool->EndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_ASSERT(vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, m_inFlightFences[currentFrame]),
              "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR presentSwapChains[] = {swapChains[0]->vkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = presentSwapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;  // Optional

    result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain(swapChains[0].get());
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    currentFrame = (currentFrame + 1) % Engine::MaxFramesInFlight;
}

void GraphicsEngine::OnWindowResized(GLFWwindow *window, int width, int height)
{
    m_framebufferResized = true;
}