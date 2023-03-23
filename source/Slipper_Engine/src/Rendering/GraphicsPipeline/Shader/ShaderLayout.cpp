#include "ShaderLayout.h"

#include "ShaderReflection.h"

namespace Slipper
{
ShaderLayout::ShaderLayout(const std::vector<std::vector<char>> &BinaryCode)
{
    setLayouts = ShaderReflection::GetMergedDescriptorSetsLayoutData(BinaryCode);

    PopulateNamesLayoutBindings();
}

void ShaderLayout::PopulateNamesLayoutBindings()
{
    for (auto &layout_data : setLayouts) {
	    for (auto &binding : layout_data.bindings)
	    {
            namedLayoutBindings[String::to_lower(binding.name)] = &binding;
	    }
    }
}
}  // namespace Slipper
