#include "GraphicsEngine.h"

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
        SetupDefaultAssets();
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

    for (auto &shader : shaders)
    {
        shader.Destroy();
    }
}

void GraphicsEngine::SetupDefaultAssets()
{
    /* Create shader for this pipeline. */
    shaders.emplace_back(device, this, "src/Shaders/Spir-V/vert.spv", ShaderType::Vertex);
    shaders.emplace_back(device, this, "src/Shaders/Spir-V/frag.spv", ShaderType::Fragment);
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

VkCommandBuffer GraphicsEngine::SetupSimpleDraw(GraphicsPipeline &graphicsPipeline, uint32_t imageIndex)
{
    VkCommandBuffer commandBuffer = commandPools[0].BeginCommandBuffer(0);
    renderPasses[0].BeginRenderPass(&swapChains[0], imageIndex, commandBuffer, graphicsPipeline);

    vkCmdDraw(commandPools[0].vkCommandBuffers[0], 3, 1, 0, 0);

    renderPasses[0].EndRenderPass(commandBuffer);
    commandPools[0].EndCommandBuffer(commandBuffer);

    return commandBuffer;
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