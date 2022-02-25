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
    GraphicsPipeline() = delete;
    GraphicsPipeline(Device &device);
    GraphicsPipeline(Device &device, VkPipelineShaderStageCreateInfo *shaderStages, VkExtent2D extent, RenderPass *renderPass);
    void Create(VkPipelineShaderStageCreateInfo *shaderStages, VkExtent2D extent, RenderPass *renderPass);
    void Destroy();

public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;

    std::vector<CommandPool> commandPools;
};