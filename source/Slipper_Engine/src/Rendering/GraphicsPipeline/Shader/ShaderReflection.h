#pragma once
#include <memory>

#include "Engine.h"


enum class ShaderType;
struct ShaderMember;
struct DescriptorSetLayoutBinding;
class Shader;
struct ModuleDescriptorSetLayoutInfo;
struct SpvReflectDescriptorBinding;
struct SpvReflectDescriptorSet;
struct SpvReflectBlockVariable;

class ShaderReflection
{
 public:
    static std::unique_ptr<ModuleDescriptorSetLayoutInfo> CreateShaderBindingInfo(
        const void *SpirvCode, size_t SpirvCodeByteCount);

    static void PopulateDescriptorSetLayoutInfo(ShaderType ShaderType,
                                          const std::vector<SpvReflectDescriptorSet *> &ReflSets,
                                          std::unique_ptr<ModuleDescriptorSetLayoutInfo> &LayoutInfo);

    static DescriptorSetLayoutBinding PopulateDescriptorSetLayoutBinding(
        ShaderType ShaderType,
        const SpvReflectDescriptorBinding *ReflSetBinding);

    static void PopulateShaderMember(ShaderMember &Member,
                                     const SpvReflectBlockVariable *ReflMember);

    static void create_descriptor_sets(const size_t Count,
                                       const VkDescriptorSetLayout DescriptorSetLayout);
};