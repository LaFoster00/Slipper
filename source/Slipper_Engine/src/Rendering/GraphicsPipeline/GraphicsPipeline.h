#pragma once

#include "RenderPass.h"

namespace Slipper
{
class Device;
class Window;
class Surface;

class GraphicsPipeline
{
 public:
    GraphicsPipeline() = delete;
    GraphicsPipeline(const std::vector<VkPipelineShaderStageCreateInfo> &ShaderStages,
                     VkExtent2D Extent,
                     const RenderPass *RenderPass,
                     const VkDescriptorSetLayout DescriptorSetLayout);
    ~GraphicsPipeline();

    void Bind(const VkCommandBuffer &CommandBuffer) const;
    void ChangeResolution(const VkExtent2D &Resolution);

 private:
    void Create(VkExtent2D Extent);

 public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;

 private:
    const RenderPass *m_renderPass;
    const std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};
}  // namespace Slipper