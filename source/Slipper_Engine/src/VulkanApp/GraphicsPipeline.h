#pragma once

#include "common_includes.h"
#include "Shader.h"

#include <optional>
#include <vector>

class Device;

class GraphicsPipeline
{
public:
    void Create(Device *device);
    void Destroy();

private:
    static VkPipelineVertexInputStateCreateInfo SetupVertexInputState();
    static VkPipelineInputAssemblyStateCreateInfo SetupInputAssemblyState();
    static VkPipelineViewportStateCreateInfo SetupViewportState(Device *device);
    static VkPipelineRasterizationStateCreateInfo SetupRasterizationState();
    static VkPipelineMultisampleStateCreateInfo SetupMultisampleState();
    static VkPipelineDepthStencilStateCreateInfo SetupDepthStencilState();
    static VkPipelineColorBlendStateCreateInfo SetupColorBlendState();
    static VkPipelineDynamicStateCreateInfo SetupDynamicState(std::optional<std::vector<VkDynamicState>> dynamicStates);

    void CreatePipelineLayout();

public:
    Device *owningDevice;

    std::vector<Shader> shaders;
    VkPipelineLayout pipelineLayout;
};