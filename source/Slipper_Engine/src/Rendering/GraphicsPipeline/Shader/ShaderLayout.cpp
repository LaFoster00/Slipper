#include "ShaderLayout.h"

#include <ranges>

#include "Mesh/UniformBuffer.h"
#include "ShaderReflection.h"
#include "Texture/Texture.h"
#include "common_defines.h"

std::vector<VkDescriptorSetLayoutBinding> ModuleDescriptorSetLayoutInfo::GetVkBindings() const
{
    std::vector<VkDescriptorSetLayoutBinding> vkBindings(bindings.size());
    for (int i = 0; i < vkBindings.size(); ++i) {
        vkBindings[i] = bindings[i].binding;
    }
    return vkBindings;
}

ShaderModuleLayout::ShaderModuleLayout(const std::vector<char> &BinaryCode)
{
    layoutInfo = ShaderReflection::CreateShaderBindingInfo(BinaryCode.data(), BinaryCode.size());

    PopulateNamesLayoutBindings();
}

ShaderModuleLayout::~ShaderModuleLayout()
{
}

void ShaderModuleLayout::PopulateNamesLayoutBindings()
{
    for (auto &binding : layoutInfo->bindings) {
        namedLayoutBindings.insert(std::make_pair(String::to_lower(binding.name), &binding));
    }
}
