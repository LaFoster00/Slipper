#include "GraphicsPipeline.h"

#include "common_defines.h"
#include "../Setup/Device.h"
#include "PipelineLayout.h"
#include "../Presentation/Surface.h"
#include "../../Window/Window.h"

#include <limits>
#include <algorithm>

void GraphicsPipeline::Create(Device *device, Window *window, Surface *surface)
{
    owningDevice = device;

    CreateSwapChain(window, surface);

    /* Create shader for this pipeline. */
    shaders.emplace_back("src/Shaders/Spir-V/vert.spv", ShaderType::Vertex, device, this);
    shaders.emplace_back("src/Shaders/Spir-V/frag.spv", ShaderType::Fragment, device, this);

    /* Create renderpass for this pipeline */
    renderPass.Create(device, this);

    /* Create graphicspipeline */
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = vkShaderStages.data();

    auto vertexInput = PipelineLayout::SetupVertexInputState();
    pipelineInfo.pVertexInputState = &vertexInput;

    auto inputAssembly = PipelineLayout::SetupInputAssemblyState();
    pipelineInfo.pInputAssemblyState = &inputAssembly;

    VkViewport viewport{};
    VkRect2D scissor{};
    auto viewportState = PipelineLayout::SetupViewportState(viewport, scissor, &swapChains[0]);
    pipelineInfo.pViewportState = &viewportState;

    auto rasterizer = PipelineLayout::SetupRasterizationState();
    pipelineInfo.pRasterizationState = &rasterizer;

    auto multisampling = PipelineLayout::SetupMultisampleState();
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    auto colorBlending = PipelineLayout::SetupColorBlendState(colorBlendAttachment);
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional

    vkPipelineLayout = PipelineLayout::CreatePipelineLayout(owningDevice);
    pipelineInfo.layout = vkPipelineLayout;

    pipelineInfo.renderPass = renderPass.vkRenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    VK_ASSERT(vkCreateGraphicsPipelines(device->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline), "Failed to create graphics pipeline!")

    CreateSwapChainFramebuffers();
    auto &commandPool = commandPools.emplace_back(*owningDevice);
    commandPool.CreateCommandBuffer();
}

void GraphicsPipeline::CreateSwapChain(Window *window, Surface *surface)
{
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
    VkPresentModeKHR presentMode = ChooseSwapPresentMode();
    VkExtent2D extent = ChooseSwapExtent(window);

    uint32_t imageCount = std::clamp(
        static_cast<uint32_t>(owningDevice->swapchainSupportDetails.capabilities.minImageCount + 1),
        static_cast<uint32_t>(0),
        static_cast<uint32_t>(owningDevice->swapchainSupportDetails.capabilities.maxImageCount));

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t indices[] = {owningDevice->queueFamilyIndices.graphicsFamily.value(), owningDevice->queueFamilyIndices.presentFamily.value()};

    if (owningDevice->queueFamilyIndices.graphicsFamily != owningDevice->queueFamilyIndices.presentFamily)
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

    createInfo.preTransform = owningDevice->swapchainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create new swap chain for this device
    swapChains.emplace_back(owningDevice, &createInfo, true);
}

void GraphicsPipeline::Destroy()
{
    for (auto &commandPool : commandPools)
    {
        commandPool.Destroy();
    }

    for (auto framebuffer : swapChainFramebuffers)
    {
        framebuffer.Destroy();
    }

    vkDestroyPipeline(owningDevice->logicalDevice, vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(owningDevice->logicalDevice, vkPipelineLayout, nullptr);

    renderPass.Destroy();

    for (auto &shader : shaders)
    {
        shader.Destroy();
    }

    for (auto &swapChain : swapChains)
    {
        swapChain.Destroy();
    }
}

void GraphicsPipeline::CreateSwapChainFramebuffers()
{
    for (size_t i = 0; i < swapChains[0].swapChainImageViews.size(); i++)
    {
        VkImageView *attachment = &swapChains[0].swapChainImageViews[i];
        swapChainFramebuffers.emplace_back(attachment, &swapChains[0], &renderPass);
    }
}

VkSurfaceFormatKHR GraphicsPipeline::ChooseSwapSurfaceFormat()
{
    for (const auto &availableFormat : owningDevice->swapchainSupportDetails.formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return owningDevice->swapchainSupportDetails.formats[0];
}

VkPresentModeKHR GraphicsPipeline::ChooseSwapPresentMode()
{
    for (const auto &availablePresentMode : owningDevice->swapchainSupportDetails.presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsPipeline::ChooseSwapExtent(Window *window)
{
    if (owningDevice->swapchainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return owningDevice->swapchainSupportDetails.capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window->glfwWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width,
                                        owningDevice->swapchainSupportDetails.capabilities.minImageExtent.width,
                                        owningDevice->swapchainSupportDetails.capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         owningDevice->swapchainSupportDetails.capabilities.minImageExtent.height,
                                         owningDevice->swapchainSupportDetails.capabilities.maxImageExtent.height);

        return actualExtent;
    }
}