#include "ComponentEditorRegistry.h"

namespace Slipper::Editor
{
std::optional<const EditorInfo*> EditorRegistry::TryGetEditor(const entt::id_type Id)
{
    if (EditorInfo::IdToInfoLookup().contains(Id))
        if (const auto type = EditorInfo::IdToInfoLookup().at(Id); EditorInfo::Map().contains(type)) {
            return EditorInfo::Map().at(type);
        }
    return {};
}

std::optional<const EditorInfo*> EditorRegistry::TryGetEditor(
    const entt::type_info &Type)
{
    if (EditorInfo::Map().contains(&Type)) {
        return EditorInfo::Map().at(&Type);
    }
    return {};
} 

EditorDrawFallback &EditorRegistry::GetEditorFallback()
{
    return FallbackDraw;
}
}  // namespace Slipper::Editor
