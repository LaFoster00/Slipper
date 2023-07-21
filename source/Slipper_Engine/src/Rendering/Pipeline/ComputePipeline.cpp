#include "ComputePipeline.h"

namespace Slipper
{
ComputePipeline::ComputePipeline(const vk::PipelineShaderStageCreateInfo &ComputeShader,
                                 vk::DescriptorSetLayout &Layout)
{
    vk::PipelineLayoutCreateInfo layout_create_info(vk::PipelineLayoutCreateFlags{}, Layout);
    VK_HPP_ASSERT(device.logicalDevice.createPipelineLayout(&layout_create_info, nullptr, &vkPipelineLayout),
                  "Compute Pipeline Layout Creation Failed");

    vk::ComputePipelineCreateInfo pipeline_create_info(
        vk::PipelineCreateFlags{}, ComputeShader, vkPipelineLayout);

    VK_HPP_ASSERT(device.logicalDevice.createComputePipelines(
                      VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &vkPipeline),
                  "Compute Pipeline Creation Failed")
}

ComputePipeline::~ComputePipeline()
{
    device.logicalDevice.destroyPipeline(vkPipeline, nullptr);
    device.logicalDevice.destroyPipelineLayout(vkPipelineLayout);
}
}  // namespace Slipper
