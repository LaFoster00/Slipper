#pragma once

#include "common_includes.h"

#include "Shader.h"
#include "RenderPass.h"
#include "VulkanApp/Drawing/Framebuffer.h"
#include "VulkanApp/Drawing/CommandPool.h"

#include <optional>
#include <vector>

class Device;
class Window;
class Surface;

class GraphicsPipeline
{
public:
    GraphicsPipeline() = delete;
    GraphicsPipeline(Device &device, VkPipelineShaderStageCreateInfo *shaderStages, VkExtent2D extent, RenderPass *renderPass);
    ~GraphicsPipeline();

private:
    void Create(VkPipelineShaderStageCreateInfo *shaderStages, VkExtent2D extent, RenderPass *renderPass);

public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;
};