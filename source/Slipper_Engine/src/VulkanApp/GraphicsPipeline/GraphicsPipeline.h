#pragma once

#include "common_includes.h"

#include "Shader.h"
#include "RenderPass.h"
#include "VulkanApp/Drawing/Framebuffer.h"
#include "VulkanApp/Drawing/CommnadPool.h"

#include <optional>
#include <vector>

class Device;
class Window;
class Surface;

class GraphicsPipeline
{
public:
    void Create(Device *device, Window *window, Surface *surface);
    void Destroy();

    void CreateSwapChain(Window *window, Surface *surface);

private:
    void CreateSwapChainFramebuffers();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
    VkPresentModeKHR ChooseSwapPresentMode();
    VkExtent2D ChooseSwapExtent(Window *window);

public:
    Device *owningDevice;

    std::vector<Shader> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

    RenderPass renderPass;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;

    std::vector<SwapChain> swapChains;
    std::vector<Framebuffer> swapChainFramebuffers;

    std::vector<CommandPool> commandPools;
};