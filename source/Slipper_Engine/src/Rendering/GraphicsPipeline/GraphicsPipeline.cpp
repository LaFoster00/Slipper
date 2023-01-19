#include "GraphicsPipeline.h"

#include "Window/Window.h"
#include "Setup/Device.h"
#include "PipelineLayout.h"
#include "common_defines.h"

#include <algorithm>

GraphicsPipeline::GraphicsPipeline(const std::vector<VkPipelineShaderStageCreateInfo> &ShaderStages,
                                   const VkExtent2D Extent,
                                   const RenderPass *RenderPass,
                                   const VkDescriptorSetLayout DescriptorSetLayout)
    : device(Device::Get()), m_renderPass(RenderPass), m_shaderStages(ShaderStages)
{
    vkPipelineLayout = PipelineLayout::CreatePipelineLayout(device, DescriptorSetLayout);
    Create(Extent);
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(device.logicalDevice, vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.logicalDevice, vkPipelineLayout, nullptr);
}

void GraphicsPipeline::Bind(const VkCommandBuffer &CommandBuffer) const
{
    vkCmdBindPipeline(CommandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vkGraphicsPipeline);
}

void GraphicsPipeline::ChangeResolution(const VkExtent2D& Resolution)
{
    vkDestroyPipeline(device, vkGraphicsPipeline, nullptr);

    Create(Resolution);
}

void GraphicsPipeline::Create(VkExtent2D Extent)
{
    /* Create graphicspipeline */
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipeline_info.pStages = m_shaderStages.data();

    auto vertex_input = PipelineLayout::SetupVertexInputState();
    pipeline_info.pVertexInputState = &vertex_input;

    auto input_assembly = PipelineLayout::SetupInputAssemblyState();
    pipeline_info.pInputAssemblyState = &input_assembly;

    VkViewport viewport{};
    VkRect2D scissor{};
    auto viewport_state = PipelineLayout::SetupViewportState(viewport, scissor, Extent);
    pipeline_info.pViewportState = &viewport_state;

    auto rasterizer = PipelineLayout::SetupRasterizationState();
    pipeline_info.pRasterizationState = &rasterizer;

    auto multisampling = PipelineLayout::SetupMultisampleState();
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr;  // Optional

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    auto color_blending = PipelineLayout::SetupColorBlendState(color_blend_attachment);
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr;  // Optional

    pipeline_info.layout = vkPipelineLayout;

    pipeline_info.renderPass = m_renderPass->vkRenderPass;
    pipeline_info.subpass = 0;

    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;  // Optional
    pipeline_info.basePipelineIndex = -1;               // Optional

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            device.logicalDevice, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vkGraphicsPipeline),
        "Failed to create graphics pipeline!");
}
