#include "GraphicsEngine.h"

#include <algorithm>

#include "Setup/Device.h"
#include "Window/Window.h"
#include "Presentation/Surface.h"

GraphicsEngine *GraphicsEngine::instance = nullptr;

GraphicsEngine::GraphicsEngine(Device &device, bool setupDefaultAssets) : device(device)
{
    if (instance != nullptr)
        return;

    instance = this;

    if (setupDefaultAssets)
    {
        SetupDefaultAssets();
        CreateSyncObjects();
    }
}

GraphicsEngine::~GraphicsEngine()
{
    for (auto &commandPool : commandPools)
    {
        commandPool.Destroy();
    }

    for (auto &graphicsPipeline : graphicsPipelines)
    {
        graphicsPipeline.Destroy();
    }

    for (auto &renderPass : renderPasses)
    {
        renderPass.Destroy();
    }

    for (auto &swapChain : swapChains)
    {
        swapChain.Destroy();
    }

    vkDestroySemaphore(device.logicalDevice, m_imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device.logicalDevice, m_renderFinishedSemaphore, nullptr);
    vkDestroyFence(device.logicalDevice, m_inFlightFence, nullptr);

    for (auto &shader : shaders)
    {
        shader.Destroy();
    }
}

void GraphicsEngine::SetupDefaultAssets()
{
    /* Create shader for this pipeline. */
    shaders.emplace_back(device, this, "EngineContent/Shaders/Spir-V/vert.spv", ShaderType::Vertex);
    shaders.emplace_back(device, this, "EngineContent/Shaders/Spir-V/frag.spv", ShaderType::Fragment);
}

void GraphicsEngine::CreateSwapChain(Window &window, Surface &surface)
{
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

    uint32_t indices[] = {device.queueFamilyIndices.graphicsFamily.value(), device.queueFamilyIndices.presentFamily.value()};

    if (device.queueFamilyIndices.graphicsFamily != device.queueFamilyIndices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = device.swapchainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create new swap chain for this device
    swapChains.emplace_back(device, &createInfo, true);
}

void GraphicsEngine::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_ASSERT(vkCreateSemaphore(device.logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore), "Failed to create semaphore!")
    VK_ASSERT(vkCreateSemaphore(device.logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore), "Failed to create semaphore!")
    VK_ASSERT(vkCreateFence(device.logicalDevice, &fenceInfo, nullptr, &m_inFlightFence), "Failed to create fence!")
}

GraphicsPipeline &GraphicsEngine::SetupSimpleRenderPipeline(Window &window, Surface &surface)
{
    CreateSwapChain(window, surface);

    /* Create renderpass for this pipeline */
    renderPasses.emplace_back(device, &swapChains[0].vkImageFormat);
    renderPasses[0].CreateSwapChainFramebuffers(&swapChains[0]);

    auto &pipeline = graphicsPipelines.emplace_back(device, vkShaderStages.data(), swapChains[0].vkExtent, &renderPasses[0]);

    commandPools.emplace_back(device);
    commandPools[0].CreateCommandBuffer();

    return pipeline;
}

void GraphicsEngine::SetupSimpleDraw()
{
    AddRepeatedDrawCommand([](VkCommandBuffer commandBuffer)
                           { vkCmdDraw(commandBuffer, 3, 1, 0, 0); });
}

void GraphicsEngine::AddRepeatedDrawCommand(std::function<void(VkCommandBuffer &)> command)
{
    repeatedRenderCommands.push_back(command);
}

void GraphicsEngine::DrawFrame()
{
    vkWaitForFences(device.logicalDevice, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logicalDevice, 1, &m_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.logicalDevice, swapChains[0].vkSwapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkCommandBuffer commandBuffer = commandPools[0].BeginCommandBuffer(0);
    renderPasses[0].BeginRenderPass(&swapChains[0], imageIndex, commandBuffer, graphicsPipelines[0]);

    for (auto &command : repeatedRenderCommands)
    {
        command(commandBuffer);
    }

    renderPasses[0].EndRenderPass(commandBuffer);
    commandPools[0].EndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_ASSERT(vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, m_inFlightFence), "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR presentSwapChains[] = {swapChains[0].vkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = presentSwapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(device.presentQueue, &presentInfo);
}

VkSurfaceFormatKHR GraphicsEngine::ChooseSwapSurfaceFormat()
{
    for (const auto &availableFormat : device.swapchainSupportDetails.formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return device.swapchainSupportDetails.formats[0];
}

VkPresentModeKHR GraphicsEngine::ChooseSwapPresentMode()
{
    for (const auto &availablePresentMode : device.swapchainSupportDetails.presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsEngine::ChooseSwapExtent(Window &window)
{
    if (device.swapchainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return device.swapchainSupportDetails.capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window.glfwWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width,
                                        device.swapchainSupportDetails.capabilities.minImageExtent.width,
                                        device.swapchainSupportDetails.capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         device.swapchainSupportDetails.capabilities.minImageExtent.height,
                                         device.swapchainSupportDetails.capabilities.maxImageExtent.height);

        return actualExtent;
    }
}