#pragma once

#include "spirv_reflect.h"
#include "common_includes.h"

enum class ShaderMemberType : uint32_t;
enum class ShaderType;

namespace ShaderReflectionUtil
{
VkDescriptorType to_vk_descriptor_type(SpvReflectDescriptorType Type);
ShaderMemberType to_shader_member_type(const SpvReflectTypeDescription &Type);
ShaderType to_shader_type(SpvReflectShaderStageFlagBits Stage);
VkShaderStageFlags to_shader_stage_flag(ShaderType Type);



std::string to_string_spv_storage_class(SpvStorageClass StorageClass);
std::string to_string_spv_dim(SpvDim Dim);
std::string to_string_spv_built_in(SpvBuiltIn BuiltIn);
std::string to_string_spv_image_format(SpvImageFormat Fmt);

std::string to_string_shader_stage(SpvReflectShaderStageFlagBits Stage);
std::string to_string_resource_type(SpvReflectResourceType Type);
std::string to_string_descriptor_type(SpvReflectDescriptorType Value);
std::string to_string_type_flags(SpvReflectTypeFlags TypeFlags);
std::string to_string_format(SpvReflectFormat Fmt);
std::string to_string_component_type(const SpvReflectTypeDescription &Type,
                                  uint32_t MemberDecorationFlags);
std::string to_string_type(SpvSourceLanguage src_lang, const SpvReflectTypeDescription &Type);

void print_module_info(std::ostream &Os, const SpvReflectShaderModule &Obj, const char *Indent = "");
void print_descriptor_set(std::ostream &Os,
                        const SpvReflectDescriptorSet &Obj,
                        const char *Indent = "");
void print_descriptor_binding(std::ostream &Os,
                            const SpvReflectDescriptorBinding &Obj,
                            bool WriteSet,
                            const char *Indent = "");
void print_interface_variable(std::ostream &Os,
                            SpvSourceLanguage SrcLang,
                            const SpvReflectInterfaceVariable &Obj,
                            const char *Indent);
}
