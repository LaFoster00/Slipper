#include "PipelineLayout.h"

#include "Presentation/SwapChain.h"

#include "Mesh/Mesh.h"

namespace Slipper
{
VkPipelineLayout PipelineLayout::CreatePipelineLayout(
    const Device &Device, const VkDescriptorSetLayout DescriptorSetLayout)
{
    const std::vector descriptor_set_layouts(Engine::MAX_FRAMES_IN_FLIGHT, DescriptorSetLayout);
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 0;     // Optional
    pipeline_layout_info.pPushConstantRanges = nullptr;  // Optional

    VkPipelineLayout pipeline_layout;
    VK_ASSERT(vkCreatePipelineLayout(
                  Device.logicalDevice, &pipeline_layout_info, nullptr, &pipeline_layout),
              "Failed to create pipeline layout!");

    return pipeline_layout;
}

VkPipelineVertexInputStateCreateInfo PipelineLayout::SetupVertexInputState()
{
    const auto binding_description = Vertex::GetBindingDescription();
    const auto attribute_descriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = binding_description;

    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(
        attribute_descriptions->size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions->data();

    return vertex_input_info;
}

VkPipelineInputAssemblyStateCreateInfo PipelineLayout::SetupInputAssemblyState()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    return inputAssembly;
}

VkPipelineViewportStateCreateInfo PipelineLayout::SetupViewportState(VkViewport &Viewport,
                                                                     VkRect2D &Scissor,
                                                                     const VkExtent2D &Extent)
{
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = static_cast<float>(Extent.width);
    Viewport.height = static_cast<float>(Extent.height);
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    Scissor.offset = {0, 0};
    Scissor.extent = Extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &Viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &Scissor;

    return viewport_state;
}

VkPipelineRasterizationStateCreateInfo PipelineLayout::SetupRasterizationState()
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
    rasterizer.depthBiasClamp = 0.0f;           // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;     // Optional

    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo PipelineLayout::SetupMultisampleState()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = GraphicsSettings::Get().MSAA_SAMPLES;
    multisampling.minSampleShading = 1.0f;           // Optional
    multisampling.pSampleMask = nullptr;             // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
    multisampling.alphaToOneEnable = VK_FALSE;       // Optional

    return multisampling;
}

VkPipelineDepthStencilStateCreateInfo PipelineLayout::SetupDepthStencilState()
{
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = VK_TRUE;
    depth_stencil_state.depthWriteEnable = VK_TRUE;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.minDepthBounds = 0.0f;
    depth_stencil_state.maxDepthBounds = 1.f;
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.front = {};
    depth_stencil_state.back = {};

    return depth_stencil_state;
}

/* Pass in empty colorblendattachementstate */
VkPipelineColorBlendStateCreateInfo PipelineLayout::SetupColorBlendState(
    VkPipelineColorBlendAttachmentState &ColorBlendAttachment)
{
    /* Blending needs to be setup on a per framebuffer basis! Only one used currently therefore
     * hardcoded. */
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;
    ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;  // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &ColorBlendAttachment;
    color_blending.blendConstants[0] = 0.0f;  // Optional
    color_blending.blendConstants[1] = 0.0f;  // Optional
    color_blending.blendConstants[2] = 0.0f;  // Optional
    color_blending.blendConstants[3] = 0.0f;  // Optional

    return color_blending;
}

VkPipelineDynamicStateCreateInfo PipelineLayout::SetupDynamicState(
    const std::optional<std::vector<VkDynamicState>> DynamicStates)
{
    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    if (DynamicStates.has_value()) {
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(DynamicStates.value().size());
        dynamic_state.pDynamicStates = DynamicStates.value().data();
    }
    else {
        dynamic_state.dynamicStateCount = 0;
        dynamic_state.pDynamicStates = nullptr;
    }

    return dynamic_state;
}
}  // namespace Slipper