#include "vk_ShaderReflection.h"

#include "spirv_reflect.h"
#include "Vulkan/vk_ShaderLayout.h"


namespace Slipper
{
struct IntermediateDSLD  // Working DescriptorSetLayoutdata
{
    uint32_t setNumber;
    std::vector<DescriptorSetLayoutBinding> bindings;
};

std::vector<DescriptorSetLayoutData> ShaderReflection::GetMergedDescriptorSetsLayoutData(
    std::vector<std::vector<char>> SpirvCodes)
{
    std::vector<IntermediateDSLD> intermediate_dslds;
    for (auto spirv_code : SpirvCodes) {
        auto new_dslds = GetDescriptorSetsLayoutData(spirv_code);
        append(intermediate_dslds, new_dslds);
    }

    std::map<uint32_t, DescriptorSetLayoutData> unique_dslds;

    for (IntermediateDSLD &intermediate_dsld : intermediate_dslds) {
        if (!unique_dslds.contains(intermediate_dsld.setNumber)) {
            auto &new_dsld = unique_dslds[intermediate_dsld.setNumber];
            new_dsld.setNumber = intermediate_dsld.setNumber;
            new_dsld.createInfo.flags = vk::DescriptorSetLayoutCreateFlags{};
            new_dsld.createInfo.pNext = nullptr;
        }
        auto &unique_dsld = unique_dslds.at(intermediate_dsld.setNumber);

        // Go through each binding and check if it is already defined and if so add its shader
        // stage to the valid use cases
        for (auto &intermediate_binding : intermediate_dsld.bindings) {
            bool contains_binding = false;
            for (auto &unique_binding : unique_dsld.bindings) {
                if (unique_binding.name == intermediate_binding.name) {
                    if (unique_binding.descriptorType == intermediate_binding.descriptorType &&
                        unique_binding.size == intermediate_binding.size &&
                        unique_binding.binding == intermediate_binding.binding &&
                        unique_binding.descriptorCount == intermediate_binding.descriptorCount) {
                        contains_binding = true;
                        unique_binding.stageFlags &= intermediate_binding.stageFlags;
                    }
                    else {
                        ASSERT(true,
                               "Binding {} is defined multiple times with different properties in "
                               "the shader stages. This is not allowed.",
                               unique_binding.name)
                    }
                }
            }

            if (!contains_binding) {
                unique_dsld.bindings.push_back(intermediate_binding);
            }
        }
    }

    std::vector<DescriptorSetLayoutData> dslds;
    for (auto &unique_dsld : unique_dslds | std::ranges::views::values) {
        unique_dsld.UpdateCreateInfo();
        dslds.push_back(unique_dsld);
    }

    return dslds;
}
std::vector<IntermediateDSLD> ShaderReflection::GetDescriptorSetsLayoutData(
    const std::vector<char> &SpirvCode)
{
    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        SpirvCode.size(), SpirvCode.data(), &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    uint32_t count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectDescriptorSet *> descriptorSets(count);
    result = spvReflectEnumerateDescriptorSets(&module, &count, descriptorSets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Generate all necessary data structures to create a
    // VkDescriptorSetLayout for each descriptor set in this shader.
    std::vector set_layouts(descriptorSets.size(), IntermediateDSLD{});
    for (size_t i_set = 0; i_set < descriptorSets.size(); ++i_set) {
        const auto &[set_number, binding_count, bindings] = *(descriptorSets[i_set]);
        IntermediateDSLD &layout = set_layouts[i_set];
        layout.bindings.resize(binding_count);

        for (uint32_t i_binding = 0; i_binding < binding_count; ++i_binding) {
            const SpvReflectDescriptorBinding &refl_binding = *(bindings[i_binding]);

            auto &layout_binding = layout.bindings[i_binding];
            // Vulkan Binding Info
            {
                VkDescriptorSetLayoutBinding &vk_layout_binding = layout_binding.GetVkBinding();
                vk_layout_binding.binding = refl_binding.binding;
                vk_layout_binding.descriptorType = static_cast<VkDescriptorType>(
                    refl_binding.descriptor_type);
                vk_layout_binding.descriptorCount = 1;
                for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
                    vk_layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
                }
                vk_layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(
                    module.shader_stage);
            }
            layout_binding.name = std::string(refl_binding.name);
            layout_binding.inputAttachmentIndex = refl_binding.input_attachment_index;
            layout_binding.set = refl_binding.set;
            layout_binding.resourceType = static_cast<ShaderResourceType>(
                refl_binding.resource_type);
            layout_binding.offset = refl_binding.block.offset;
            layout_binding.absoluteOffset = refl_binding.block.absolute_offset;
            layout_binding.size = refl_binding.block.size;
            layout_binding.paddedSize = refl_binding.block.padded_size;
        }
        layout.setNumber = set_number;
    }
    // Nothing further is done with set_layouts in this sample; in a real
    // application they would be merged with similar structures from other shader
    // stages and/or pipelines to create a VkPipelineLayout.
    return set_layouts;
}
}  // namespace Slipper