#pragma once

namespace Slipper
{
class ComputePipeline : DeviceDependentObject
{
 public:
    explicit ComputePipeline(const vk::PipelineShaderStageCreateInfo &ComputeShader,
                             vk::DescriptorSetLayout &Layout);

    ~ComputePipeline();

 public:
    vk::Pipeline vkComputePipeline = VK_NULL_HANDLE;
};
}  // namespace Slipper