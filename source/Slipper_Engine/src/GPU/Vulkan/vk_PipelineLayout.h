#pragma once

struct VkViewport;
struct VkRect2D;
struct VkPipelineColorBlendAttachmentState;

namespace Slipper::GPU::Vulkan
{
class SwapChain;
class VKDevice;

class PipelineLayout
{
 public:
    static VkPipelineLayout CreatePipelineLayout(
        const VKDevice &Device, const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);

    static VkPipelineVertexInputStateCreateInfo SetupVertexInputState();

    static VkPipelineInputAssemblyStateCreateInfo SetupInputAssemblyState();

    static VkPipelineViewportStateCreateInfo SetupViewportState(VkViewport &Viewport,
                                                                VkRect2D &Scissor,
                                                                const VkExtent2D &Extent);
    static VkPipelineRasterizationStateCreateInfo SetupRasterizationState();

    static VkPipelineMultisampleStateCreateInfo SetupMultisampleState();

    static VkPipelineDepthStencilStateCreateInfo SetupDepthStencilState();

    static VkPipelineColorBlendStateCreateInfo SetupColorBlendState(
        VkPipelineColorBlendAttachmentState &ColorBlendAttachment);

    static VkPipelineDynamicStateCreateInfo SetupDynamicState(
        std::optional<std::vector<VkDynamicState>> DynamicStates);
};
}  // namespace Slipper