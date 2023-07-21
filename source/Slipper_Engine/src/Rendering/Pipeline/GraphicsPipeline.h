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
                     NonOwningPtr<const RenderPass> RenderPass,
                     const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);
    ~GraphicsPipeline();

    void Bind(const VkCommandBuffer &CommandBuffer, VkExtent2D Extent) const;

 private:
    void Create();

 public:
    Device &device;

    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkGraphicsPipeline;

 private:
    NonOwningPtr<const RenderPass> m_renderPass;
    const std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    VkViewport m_vkViewport;
    VkRect2D m_vkScissor;
};
}  // namespace Slipper