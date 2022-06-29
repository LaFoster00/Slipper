#pragma once

#include "common_includes.h"

#include "RenderPass.h"
#include "Shader.h"
#include "VulkanApp/Drawing/CommandPool.h"
#include "VulkanApp/Drawing/Framebuffer.h"

#include <optional>
#include <vector>

class Device;
class Window;
class Surface;

class GraphicsPipeline
{
 public:
    GraphicsPipeline() = delete;
    GraphicsPipeline(Device &device,
                     VkPipelineShaderStageCreateInfo *shaderStages,
                     VkExtent2D extent,
                     RenderPass *renderPass);
    ~GraphicsPipeline();

 private:
    void Create(VkPipelineShaderStageCreateInfo *shaderStages,
                VkExtent2D extent,
                RenderPass *renderPass);

 public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;
};