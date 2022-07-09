#include "ShaderReflection.h"

#include <cassert>

#include "Setup/Device.h"
#include "Shader.h"
#include "ShaderReflectionUtil.h"
#include "common_defines.h"
#include "spirv_reflect.h"

std::vector<DescriptorSetLayoutInfo *> ShaderReflection::CreateShaderBindingInfo(
    const void *SpirvCode, size_t SpirvCodeByteCount)
{
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(SpirvCodeByteCount, SpirvCode, &module);
    ASSERT(result = SPV_REFLECT_RESULT_SUCCESS,
           "Failed to create reflection data for shader code.")

    // Descriptor Sets
    uint32_t descriptor_set_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &descriptor_set_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectDescriptorSet *> refl_descriptor_sets(descriptor_set_count);
    result = spvReflectEnumerateDescriptorSets(
        &module, &descriptor_set_count, refl_descriptor_sets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<DescriptorSetLayoutInfo *> descriptor_sets_layout;

    PopulateDescriptorSetLayoutInfo(ShaderReflectionUtil::to_shader_type(module.shader_stage),
                              refl_descriptor_sets,
                              descriptor_sets_layout);

    spvReflectDestroyShaderModule(&module);

    return descriptor_sets_layout;
}

void ShaderReflection::PopulateDescriptorSetLayoutInfo(
    ShaderType ShaderType,
    const std::vector<SpvReflectDescriptorSet *> &ReflSets,
    std::vector<DescriptorSetLayoutInfo *> &LayoutInfos)
{
    LayoutInfos.reserve(ReflSets.size());
    for (const auto refl_descriptor_set : ReflSets) {
        const auto &layout_info = LayoutInfos.emplace_back(new DescriptorSetLayoutInfo());
        layout_info->setNumber = refl_descriptor_set->set;
        layout_info->bindings.resize(refl_descriptor_set->binding_count);

        for (uint32_t binding = 0; binding < refl_descriptor_set->binding_count; ++binding) {
            PopulateDescriptorSetLayoutBinding(ShaderType,
                                               layout_info->bindings[binding],
                                               refl_descriptor_set->bindings[binding]);
        }
    }
}

void ShaderReflection::PopulateDescriptorSetLayoutBinding(
    ShaderType ShaderType,
    DescriptorSetLayoutBinding &SetBinding,
    const SpvReflectDescriptorBinding *ReflSetBinding)
{
    SetBinding.name = ReflSetBinding->name;
    SetBinding.size = ReflSetBinding->block.size;
    SetBinding.paddedSize = ReflSetBinding->block.padded_size;
    SetBinding.members.resize(ReflSetBinding->block.member_count);

    SetBinding.binding.binding = ReflSetBinding->binding;
    SetBinding.binding.descriptorType = static_cast<VkDescriptorType>(
        ReflSetBinding->descriptor_type);
    SetBinding.binding.descriptorCount = ReflSetBinding->count;
    SetBinding.binding.pImmutableSamplers = nullptr;
    SetBinding.binding.stageFlags |= ShaderReflectionUtil::to_shader_stage_flag(ShaderType);

    for (uint32_t i = 0; i < ReflSetBinding->block.member_count; ++i) {
        PopulateShaderMember(SetBinding.members[i], &ReflSetBinding->block.members[i]);
    }
}

void ShaderReflection::PopulateShaderMember(ShaderMember &Member,
                                            const SpvReflectBlockVariable *ReflMember)
{
    Member.name = ReflMember->name;
    Member.size = ReflMember->size;
    Member.paddedSize = ReflMember->padded_size;
    Member.type = ShaderReflectionUtil::to_shader_member_type(*ReflMember->type_description);
    Member.members.resize(ReflMember->member_count);
    for (int i = 0; i < ReflMember->member_count; ++i) {
        PopulateShaderMember(Member.members[i], &ReflMember->members[i]);
    }
}

void ShaderReflection::create_descriptor_sets(const size_t Count,
                                              const VkDescriptorSetLayout DescriptorSetLayout)
{
}
