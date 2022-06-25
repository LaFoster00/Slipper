#pragma once

#include "common_includes.h"

#include <optional>

struct VkViewport;
struct VkRect2D;
struct VkPipelineColorBlendAttachmentState;
class SwapChain;
class Device;

class PipelineLayout
{
public:
    static VkPipelineLayout CreatePipelineLayout(Device &device);

    static VkPipelineVertexInputStateCreateInfo SetupVertexInputState();
    static VkPipelineInputAssemblyStateCreateInfo SetupInputAssemblyState();
    static VkPipelineViewportStateCreateInfo SetupViewportState(VkViewport &viewport, VkRect2D &scissor, VkExtent2D &extent);
    static VkPipelineRasterizationStateCreateInfo SetupRasterizationState();
    static VkPipelineMultisampleStateCreateInfo SetupMultisampleState();
    static VkPipelineDepthStencilStateCreateInfo SetupDepthStencilState();
    static VkPipelineColorBlendStateCreateInfo SetupColorBlendState(VkPipelineColorBlendAttachmentState &colorBlendAttachment);
    static VkPipelineDynamicStateCreateInfo SetupDynamicState(std::optional<std::vector<VkDynamicState>> dynamicStates);
};