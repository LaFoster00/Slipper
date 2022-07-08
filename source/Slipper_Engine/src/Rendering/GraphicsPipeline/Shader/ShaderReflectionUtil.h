#pragma once

#include "spirv_reflect.h"

namespace ShaderReflectionUtil
{
std::string ToStringSpvStorageClass(SpvStorageClass storage_class);
std::string ToStringSpvDim(SpvDim dim);
std::string ToStringSpvBuiltIn(SpvBuiltIn value);
std::string ToStringSpvImageFormat(SpvImageFormat fmt);

std::string ToStringShaderStage(SpvReflectShaderStageFlagBits stage);
std::string ToStringResourceType(SpvReflectResourceType type);
std::string ToStringDescriptorType(SpvReflectDescriptorType value);
std::string ToStringTypeFlags(SpvReflectTypeFlags type_flags);
std::string ToStringFormat(SpvReflectFormat fmt);
std::string ToStringComponentType(const SpvReflectTypeDescription &type,
                                  uint32_t member_decoration_flags);
std::string ToStringType(SpvSourceLanguage src_lang, const SpvReflectTypeDescription &type);

void PrintModuleInfo(std::ostream &os, const SpvReflectShaderModule &obj, const char *indent = "");
void PrintDescriptorSet(std::ostream &os,
                        const SpvReflectDescriptorSet &obj,
                        const char *indent = "");
void PrintDescriptorBinding(std::ostream &os,
                            const SpvReflectDescriptorBinding &obj,
                            bool write_set,
                            const char *indent = "");
void PrintInterfaceVariable(std::ostream &os,
                            SpvSourceLanguage src_lang,
                            const SpvReflectInterfaceVariable &obj,
                            const char *indent);
}