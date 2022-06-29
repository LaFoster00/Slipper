#include "GraphicsPipeline.h"

#include "../../Window/Window.h"
#include "../Presentation/Surface.h"
#include "../Setup/Device.h"
#include "PipelineLayout.h"
#include "common_defines.h"

#include <algorithm>
#include <limits>

GraphicsPipeline::GraphicsPipeline(Device &device,
                                   VkPipelineShaderStageCreateInfo *shaderStages,
                                   VkExtent2D extent,
                                   RenderPass *renderPass)
    : device(device)
{
    Create(shaderStages, extent, renderPass);
}

void GraphicsPipeline::Create(VkPipelineShaderStageCreateInfo *shaderStages,
                              VkExtent2D extent,
                              RenderPass *renderPass)
{
    /* Create graphicspipeline */
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    auto vertexInput = PipelineLayout::SetupVertexInputState();
    pipelineInfo.pVertexInputState = &vertexInput;

    auto inputAssembly = PipelineLayout::SetupInputAssemblyState();
    pipelineInfo.pInputAssemblyState = &inputAssembly;

    VkViewport viewport{};
    VkRect2D scissor{};
    auto viewportState = PipelineLayout::SetupViewportState(viewport, scissor, extent);
    pipelineInfo.pViewportState = &viewportState;

    auto rasterizer = PipelineLayout::SetupRasterizationState();
    pipelineInfo.pRasterizationState = &rasterizer;

    auto multisampling = PipelineLayout::SetupMultisampleState();
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    auto colorBlending = PipelineLayout::SetupColorBlendState(colorBlendAttachment);
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;  // Optional

    vkPipelineLayout = PipelineLayout::CreatePipelineLayout(device);
    pipelineInfo.layout = vkPipelineLayout;

    pipelineInfo.renderPass = renderPass->vkRenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
    pipelineInfo.basePipelineIndex = -1;               // Optional

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            device.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline),
        "Failed to create graphics pipeline!");
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(device.logicalDevice, vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.logicalDevice, vkPipelineLayout, nullptr);
}