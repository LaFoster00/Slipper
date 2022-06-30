#pragma once

#include "RenderPass.h"
#include "Rendering/Drawing/Framebuffer.h"

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
                     RenderPass *renderPass,
                     VkDescriptorSetLayout descriptorSet);
    ~GraphicsPipeline();

 private:
    void Create(VkPipelineShaderStageCreateInfo *shaderStages,
                VkExtent2D extent,
                RenderPass *renderPass,
                VkDescriptorSetLayout descriptorSet);

 public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;
};