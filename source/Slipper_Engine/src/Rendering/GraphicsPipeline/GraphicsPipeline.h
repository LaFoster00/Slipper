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
    GraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
                     VkExtent2D extent,
                     RenderPass *renderPass,
                     VkDescriptorSetLayout descriptorSet);
    ~GraphicsPipeline();

    void Bind(const VkCommandBuffer &commandBuffer) const;
    void ChangeResolution(const VkExtent2D &resolution);

private:
    void Create(VkExtent2D extent);

 public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;

 private:
    const RenderPass *m_renderPass;
    const std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};