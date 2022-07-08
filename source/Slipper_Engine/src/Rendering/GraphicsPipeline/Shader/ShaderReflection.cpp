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
    descriptor_sets_layout.reserve(descriptor_set_count);

    CreateDescriptorSetLayout(refl_descriptor_sets, descriptor_sets_layout);

    spvReflectDestroyShaderModule(&module);

    return descriptor_sets_layout;
}

void ShaderReflection::CreateDescriptorSetLayout(
    const std::vector<SpvReflectDescriptorSet *> &ReflSets,
    std::vector<DescriptorSetLayoutInfo *> &LayoutInfos)
{
    for (const auto refl_descriptor_set : ReflSets) {
        const auto layout_info = LayoutInfos.emplace_back(new DescriptorSetLayoutInfo());
        layout_info->setNumber = refl_descriptor_set->set;
        layout_info->bindings.reserve(refl_descriptor_set->binding_count);

        for (uint32_t binding = 0; binding < refl_descriptor_set->binding_count; ++binding) {
            PopulateDescriptorSetLayoutBinding(layout_info->bindings[binding],
                                               refl_descriptor_set->bindings[binding]);
        }

        layout_info->createInfo = {};
        layout_info->createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        auto bindings = layout_info->GetVkBindings();
        layout_info->createInfo.bindingCount = bindings.size();
        layout_info->createInfo.pBindings = bindings.data();

        vkCreateDescriptorSetLayout(
            Device::Get(), &layout_info->createInfo, nullptr, &layout_info->layout);
    }
}

void ShaderReflection::PopulateDescriptorSetLayoutBinding(
    DescriptorSetLayoutBinding &SetBinding, const SpvReflectDescriptorBinding *ReflSetBinding)
{
    SetBinding.name = ReflSetBinding->name;
    SetBinding.descriptorType = static_cast<VkDescriptorType>(ReflSetBinding->descriptor_type);
    SetBinding.size = ReflSetBinding->block.size;
    SetBinding.padded_size = ReflSetBinding->block.padded_size;
    SetBinding.members.resize(ReflSetBinding->block.member_count);
    for (int i = 0; i < ReflSetBinding->block.member_count; ++i) {
        PopulateShaderMember(SetBinding.members[i], &ReflSetBinding->block.members[i]);
    }
}

void ShaderReflection::PopulateShaderMember(ShaderMember &Member,
                                            const SpvReflectBlockVariable *ReflMember)
{
    Member.name = ReflMember->name;
    Member.size = ReflMember->size;
    Member.padded_size = ReflMember->padded_size;
    Member.type = static_cast<ShaderMemberType>(ReflMember->type_description->type_flags);
    Member.members.resize(ReflMember->member_count);
    for (int i = 0; i < ReflMember->member_count; ++i) {
        PopulateShaderMember(Member.members[i], &ReflMember->members[i]);
    }
}

void ShaderReflection::create_descriptor_sets(const size_t Count,
                                              const VkDescriptorSetLayout DescriptorSetLayout)
{
}
