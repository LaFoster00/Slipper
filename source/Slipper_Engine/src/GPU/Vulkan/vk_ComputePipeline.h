#pragma once
#include "vk_DeviceDependentObject.h"

namespace Slipper::GPU::Vulkan
{
    class ComputePipeline : DeviceDependentObject
    {
     public:
        explicit ComputePipeline(const vk::PipelineShaderStageCreateInfo &ComputeShader,
                                 vk::DescriptorSetLayout &Layout);

        ~ComputePipeline();

        operator vk::Pipeline() const
        {
            return vkPipeline;
        }

     public:
        vk::Pipeline vkPipeline = VK_NULL_HANDLE;
        vk::PipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
    };
}  // namespace Slipper::GPU::Vulkan
