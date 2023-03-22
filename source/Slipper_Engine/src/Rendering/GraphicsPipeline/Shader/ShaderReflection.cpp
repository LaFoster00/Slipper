#include "ShaderReflection.h"

#include "Shader.h"
#include "spirv_reflect.h"

namespace Slipper
{
std::vector<DescriptorSetLayoutData> ShaderReflection::GetDescriptorSetsLayoutData(
    const void *SpirvCode, size_t SpirvCodeByteCount)
{
    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(SpirvCodeByteCount, SpirvCode, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    uint32_t count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectDescriptorSet *> descriptorSets(count);
    result = spvReflectEnumerateDescriptorSets(&module, &count, descriptorSets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Generate all necessary data structures to create a
    // VkDescriptorSetLayout for each descriptor set in this shader.
    std::vector set_layouts(descriptorSets.size(), DescriptorSetLayoutData{});
    for (size_t i_set = 0; i_set < descriptorSets.size(); ++i_set) {
        const auto &[set_number, binding_count, bindings] = *(descriptorSets[i_set]);
        DescriptorSetLayoutData &layout = set_layouts[i_set];
        layout.bindings.resize(binding_count);

        for (uint32_t i_binding = 0; i_binding < binding_count; ++i_binding) {
            const SpvReflectDescriptorBinding &refl_binding = *(bindings[i_binding]);

            auto &layout_binding = layout.bindings[i_binding];
            // Vulkan Binding Info
            {
                VkDescriptorSetLayoutBinding &vk_layout_binding =
                    layout_binding.GetVkBinding();
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
        layout.createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout.createInfo.bindingCount = binding_count;
        // Extract Vulkan bindings for create info
        {
            for (auto binding : layout.bindings) {
                layout.vkBindings.push_back(binding.GetVkBinding());
            }
            layout.createInfo.pBindings = layout.vkBindings.data();
        }
    }
    // Nothing further is done with set_layouts in this sample; in a real
    // application they would be merged with similar structures from other shader
    // stages and/or pipelines to create a VkPipelineLayout.
    return set_layouts;
}

void ShaderReflection::create_descriptor_sets(const size_t Count,
                                              const VkDescriptorSetLayout DescriptorSetLayout)
{
}
}  // namespace Slipper