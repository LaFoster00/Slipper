#pragma once

#include "Engine.h"

#include <optional>

struct VkViewport;
struct VkRect2D;
struct VkPipelineColorBlendAttachmentState;

namespace Slipper
{
class SwapChain;
class Device;

class PipelineLayout
{
 public:
    static VkPipelineLayout CreatePipelineLayout(const Device &Device,
                                                 const VkDescriptorSetLayout DescriptorSetLayout);

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