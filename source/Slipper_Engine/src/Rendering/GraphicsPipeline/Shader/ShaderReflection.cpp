#include "ShaderReflection.h"

#include <cassert>

#include "Setup/Device.h"
#include "Shader.h"
#include "ShaderReflectionUtil.h"
#include "common_defines.h"
#include "spirv_reflect.h"

std::unique_ptr<ModuleDescriptorSetLayoutInfo> ShaderReflection::CreateShaderBindingInfo(
    const void *SpirvCode, size_t SpirvCodeByteCount)
{
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(SpirvCodeByteCount, SpirvCode, &module);
    ASSERT(result,
           "Failed to create reflection data for shader code.")

    // Descriptor Sets
    uint32_t descriptor_set_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &descriptor_set_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectDescriptorSet *> refl_descriptor_sets(descriptor_set_count);
    result = spvReflectEnumerateDescriptorSets(
        &module, &descriptor_set_count, refl_descriptor_sets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    auto descriptor_sets_layout = std::make_unique<ModuleDescriptorSetLayoutInfo>();

    PopulateDescriptorSetLayoutInfo(ShaderReflectionUtil::to_shader_type(module.shader_stage),
                              refl_descriptor_sets,
                              descriptor_sets_layout);

    spvReflectDestroyShaderModule(&module);

    return descriptor_sets_layout;
}

void ShaderReflection::PopulateDescriptorSetLayoutInfo(
    const ShaderType ShaderType,
    const std::vector<SpvReflectDescriptorSet *> &ReflSets,
    std::unique_ptr<ModuleDescriptorSetLayoutInfo> &LayoutInfo)
{
    for (const auto refl_descriptor_set : ReflSets) {

        for (uint32_t binding = 0; binding < refl_descriptor_set->binding_count; ++binding) {
            LayoutInfo->bindings.push_back(
            PopulateDescriptorSetLayoutBinding(ShaderType,
                                               refl_descriptor_set->bindings[binding]));
        }
    }
}

DescriptorSetLayoutBinding ShaderReflection::PopulateDescriptorSetLayoutBinding(
    const ShaderType ShaderType,
    const SpvReflectDescriptorBinding *ReflSetBinding)
{
    DescriptorSetLayoutBinding set_binding{};
    set_binding.name = ReflSetBinding->name;
    set_binding.setNumber = ReflSetBinding->set;
    set_binding.size = ReflSetBinding->block.size;
    set_binding.paddedSize = ReflSetBinding->block.padded_size;
    set_binding.members.resize(ReflSetBinding->block.member_count);

    set_binding.binding.binding = ReflSetBinding->binding;
    set_binding.binding.descriptorType = static_cast<VkDescriptorType>(
        ReflSetBinding->descriptor_type);
    set_binding.binding.descriptorCount = ReflSetBinding->count;
    set_binding.binding.pImmutableSamplers = nullptr;
    set_binding.binding.stageFlags |= ShaderReflectionUtil::to_shader_stage_flag(ShaderType);

    for (uint32_t i = 0; i < ReflSetBinding->block.member_count; ++i) {
        PopulateShaderMember(set_binding.members[i], &ReflSetBinding->block.members[i]);
    }

    return set_binding;
}

void ShaderReflection::PopulateShaderMember(ShaderMember &Member,
                                            const SpvReflectBlockVariable *ReflMember)
{
    Member.name = ReflMember->name;
    Member.size = ReflMember->size;
    Member.paddedSize = ReflMember->padded_size;
    Member.type = ShaderReflectionUtil::to_shader_member_type(*ReflMember->type_description);
    Member.members.resize(ReflMember->member_count);
    for (uint32_t i = 0; i < ReflMember->member_count; ++i) {
        PopulateShaderMember(Member.members[i], &ReflMember->members[i]);
    }
}

void ShaderReflection::create_descriptor_sets(const size_t Count,
                                              const VkDescriptorSetLayout DescriptorSetLayout)
{
}
