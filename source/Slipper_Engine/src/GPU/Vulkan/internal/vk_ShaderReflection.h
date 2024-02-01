#pragma once
struct SpvReflectDescriptorBinding;
struct SpvReflectDescriptorSet;
struct SpvReflectBlockVariable;

namespace Slipper
{
	struct IntermediateDSLD;
	struct DescriptorSetLayoutData;

class ShaderReflection
{
 public:
    static std::vector<DescriptorSetLayoutData> GetMergedDescriptorSetsLayoutData(
        std::vector<std::vector<char>> SpirvCodes);

 private:
    static std::vector<IntermediateDSLD> GetDescriptorSetsLayoutData(
        const std::vector<char> &SpirvCode);
};
}  // namespace Slipper