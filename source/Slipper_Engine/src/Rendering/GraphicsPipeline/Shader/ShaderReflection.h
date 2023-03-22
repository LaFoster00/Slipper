#pragma once
struct SpvReflectDescriptorBinding;
struct SpvReflectDescriptorSet;
struct SpvReflectBlockVariable;

namespace Slipper
{
struct DescriptorSetLayoutData;

class ShaderReflection
{
 public:
    static std::vector<DescriptorSetLayoutData> GetDescriptorSetsLayoutData(
        const void *SpirvCode, size_t SpirvCodeByteCount);

    static void create_descriptor_sets(const size_t Count,
                                       const VkDescriptorSetLayout DescriptorSetLayout);
};
}  // namespace Slipper