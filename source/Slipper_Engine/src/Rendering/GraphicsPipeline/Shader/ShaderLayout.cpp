#include "ShaderLayout.h"

#include "ShaderReflection.h"

namespace Slipper
{
ShaderModuleLayout::ShaderModuleLayout(const std::vector<char> &BinaryCode)
{
    setLayouts = ShaderReflection::GetDescriptorSetsLayoutData(BinaryCode.data(), BinaryCode.size());

    PopulateNamesLayoutBindings();
}

void ShaderModuleLayout::PopulateNamesLayoutBindings()
{
    for (auto &layout_data : setLayouts) {
	    for (auto &binding : layout_data.bindings)
	    {
            namedLayoutBindings[String::to_lower(binding.name)] = &binding;
	    }
    }
}
}  // namespace Slipper
