#include "GraphicsPipeline.h"

#include "Window/Window.h"
#include "Setup/Device.h"
#include "PipelineLayout.h"
#include "common_defines.h"

#include <algorithm>

GraphicsPipeline::GraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
                                   VkExtent2D extent,
                                   RenderPass *renderPass,
                                   const VkDescriptorSetLayout descriptorSet)
    : device(Device::Get()), m_renderPass(renderPass), m_shaderStages(shaderStages)
{
    vkPipelineLayout = PipelineLayout::CreatePipelineLayout(device, descriptorSet);
    Create(extent);
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(device.logicalDevice, vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.logicalDevice, vkPipelineLayout, nullptr);
}

void GraphicsPipeline::Bind(const VkCommandBuffer &commandBuffer) const
{
    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vkGraphicsPipeline);
}

void GraphicsPipeline::ChangeResolution(const VkExtent2D& resolution)
{
    vkDestroyPipeline(device, vkGraphicsPipeline, nullptr);

    Create(resolution);
}

void GraphicsPipeline::Create(VkExtent2D extent)
{
    /* Create graphicspipeline */
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineInfo.pStages = m_shaderStages.data();

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

    pipelineInfo.layout = vkPipelineLayout;

    pipelineInfo.renderPass = m_renderPass->vkRenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
    pipelineInfo.basePipelineIndex = -1;               // Optional

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            device.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline),
        "Failed to create graphics pipeline!");
}
