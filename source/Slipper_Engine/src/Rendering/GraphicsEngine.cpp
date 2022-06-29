
#include "GraphicsEngine.h"

#include <algorithm>
#include <filesystem>

#include "Mesh/Mesh.h"
#include "Mesh/VertexBuffer.h"
#include "Presentation/Surface.h"
#include "Setup/Device.h"
#include "Window/Window.h"

GraphicsEngine *GraphicsEngine::instance = nullptr;

GraphicsEngine::GraphicsEngine(Device &device, bool setupDefaultAssets) : device(device)
{
    if (instance != nullptr)
        return;

    instance = this;

    if (setupDefaultAssets) {
        SetupDefaultAssets();
        CreateSyncObjects();
    }

    commandPool = new CommandPool(device, Engine::MaxFramesInFlight);
}

GraphicsEngine::~GraphicsEngine()
{
    vertexBuffers.clear();

    renderPasses.clear();
    graphicsPipelines.clear();
    swapChains.clear();

    delete commandPool;

    for (const auto m_imageAvailableSemaphore : m_imageAvailableSemaphores) {
        vkDestroySemaphore(device.logicalDevice, m_imageAvailableSemaphore, nullptr);
    }

    for (const auto m_renderFinishedSemaphore : m_renderFinishedSemaphores) {
        vkDestroySemaphore(device.logicalDevice, m_renderFinishedSemaphore, nullptr);
    }

    for (const auto m_inFlightFence : m_inFlightFences) {
        vkDestroyFence(device.logicalDevice, m_inFlightFence, nullptr);
    }

    for (auto &shader : shaders) {
        shader.Destroy();
    }
}

SwapChain *GraphicsEngine::CreateSwapChain(Window &window, Surface &surface)
{
    device.QuerySwapChainSupport(&surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
    VkPresentModeKHR presentMode = ChooseSwapPresentMode();
    VkExtent2D extent = ChooseSwapExtent(window);

    uint32_t imageCount = std::clamp(
        static_cast<uint32_t>(device.swapchainSupportDetails.capabilities.minImageCount + 1),
        static_cast<uint32_t>(0),
        static_cast<uint32_t>(device.swapchainSupportDetails.capabilities.maxImageCount));

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t indices[] = {device.queueFamilyIndices.graphicsFamily.value(),
                          device.queueFamilyIndices.presentFamily.value()};

    if (device.queueFamilyIndices.graphicsFamily != device.queueFamilyIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = device.swapchainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create new swap chain for this device and insert it into the mapping
    auto newSwapDependency = swapChainDependencies.insert(std::make_pair(
        swapChains.emplace_back(std::make_unique<SwapChain>(device, &createInfo, true)).get(),
        std::tuple<Window &, Surface &, std::vector<RenderPass *>>(window, surface, {})));
    return newSwapDependency.first->first;
}

void GraphicsEngine::CleanupSwapChain(SwapChain *SwapChain)
{
    for (auto &renderPass : renderPasses) {
        if (renderPass->DestroySwapChainFramebuffers(SwapChain) &&
            graphicsPipelines.contains(renderPass.get())) {
            graphicsPipelines.erase(renderPass.get());
        }
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
    auto currentSCD = swapChainDependencies.at(SwapChain);
    glfwGetFramebufferSize(std::get<0>(currentSCD).glfwWindow, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(std::get<0>(currentSCD).glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.logicalDevice);

    const auto currentSwapChainDependencies = swapChainDependencies.at(SwapChain);

    CleanupSwapChain(SwapChain);

    for (const auto RenderPass : std::get<2>(currentSwapChainDependencies)) {
        SetupSimpleRenderPipelineForRenderPass(std::get<0>(currentSwapChainDependencies),
                                               std::get<1>(currentSwapChainDependencies),
                                               RenderPass);
    }
}

void GraphicsEngine::SetupDefaultAssets()
{
    /* Create shader for this pipeline. */
    shaders.emplace_back(
        device, this, "./EngineContent/Shaders/Spir-V/Basic.vert.spv", ShaderType::Vertex);
    shaders.emplace_back(
        device, this, "./EngineContent/Shaders/Spir-V/Basic.frag.spv", ShaderType::Fragment);
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

RenderPass *GraphicsEngine::CreateRenderPass()
{
    return renderPasses
        .emplace_back(std::make_unique<RenderPass>(device, ChooseSwapSurfaceFormat().format))
        .get();
}

GraphicsPipeline *GraphicsEngine::SetupSimpleRenderPipelineForRenderPass(Window &window,
                                                                         Surface &surface,
                                                                         RenderPass *RenderPass)
{
    const auto SwapChain = CreateSwapChain(window, surface);

    /* Create renderpass for this pipeline */
    RenderPass->CreateSwapChainFramebuffers(SwapChain);
    std::get<2>(swapChainDependencies.at(SwapChain)).push_back(RenderPass);

    auto pipeline = graphicsPipelines.insert(
        std::make_pair(RenderPass,
                       std::make_unique<GraphicsPipeline>(
                           device, vkShaderStages.data(), swapChains[0]->vkExtent, RenderPass)));

    vertexBuffers.emplace_back(std::make_unique<VertexBuffer>(
        device, Mesh::DebugTriangleVertices.data(), Mesh::DebugTriangleVertices.size()));

    return pipeline.first->second.get();
}

void GraphicsEngine::SetupSimpleDraw()
{
    AddRepeatedDrawCommand([=, this](VkCommandBuffer commandBuffer) {
	    const VkBuffer vertexBuffers[] = {*this->vertexBuffers[0]};
	    constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdDraw(commandBuffer, static_cast<uint32_t>(this->vertexBuffers[0]->numVertices), 1, 0, 0);
    });
}

void GraphicsEngine::AddRepeatedDrawCommand(std::function<void(VkCommandBuffer &)> command)
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

    VkCommandBuffer commandBuffer = commandPool->BeginCommandBuffer(currentFrame);
    renderPasses[0]->BeginRenderPass(swapChains[0].get(), imageIndex, commandBuffer);

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipelines.at(renderPasses[0].get())->vkGraphicsPipeline);

    for (auto &command : repeatedRenderCommands) {
        command(commandBuffer);
    }

    renderPasses[0]->EndRenderPass(commandBuffer);
    commandPool->EndCommandBuffer(commandBuffer);

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

VkSurfaceFormatKHR GraphicsEngine::ChooseSwapSurfaceFormat()
{
    for (const auto &availableFormat : device.swapchainSupportDetails.formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return device.swapchainSupportDetails.formats[0];
}

VkPresentModeKHR GraphicsEngine::ChooseSwapPresentMode()
{
    for (const auto &availablePresentMode : device.swapchainSupportDetails.presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsEngine::ChooseSwapExtent(Window &window)
{
    if (device.swapchainSupportDetails.capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return device.swapchainSupportDetails.capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window.glfwWindow, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(
            actualExtent.width,
            device.swapchainSupportDetails.capabilities.minImageExtent.width,
            device.swapchainSupportDetails.capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
            actualExtent.height,
            device.swapchainSupportDetails.capabilities.minImageExtent.height,
            device.swapchainSupportDetails.capabilities.maxImageExtent.height);

        return actualExtent;
    }
}