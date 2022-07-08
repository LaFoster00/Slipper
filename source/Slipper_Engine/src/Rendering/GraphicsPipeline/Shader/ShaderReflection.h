#pragma once
#include <memory>

#include "common_includes.h"

struct ShaderMember;
struct DescriptorSetLayoutBinding;
class Shader;
struct DescriptorSetLayoutInfo;
struct SpvReflectDescriptorBinding;
struct SpvReflectDescriptorSet;
struct SpvReflectBlockVariable;

class ShaderReflection
{
 public:
    static std::vector<DescriptorSetLayoutInfo *> CreateShaderBindingInfo(
        const void *SpirvCode, size_t SpirvCodeByteCount);

    static void CreateDescriptorSetLayout(const std::vector<SpvReflectDescriptorSet *> &ReflSets,
                                          std::vector<DescriptorSetLayoutInfo *> &LayoutInfos);

    static void PopulateDescriptorSetLayoutBinding(DescriptorSetLayoutBinding &SetBinding,
                                                   const SpvReflectDescriptorBinding *ReflSetBinding);

    static void PopulateShaderMember(ShaderMember &Member, const SpvReflectBlockVariable *ReflMember);

    static void create_descriptor_sets(const size_t Count,
                                       const VkDescriptorSetLayout DescriptorSetLayout);
};