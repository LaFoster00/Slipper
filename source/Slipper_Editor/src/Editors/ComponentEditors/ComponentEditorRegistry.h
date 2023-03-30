#pragma once
#include "IComponentEditor.h"

namespace Slipper
{
struct Transform;
}

namespace Slipper::Editor
{

class EditorRegistry
{
 public:
    // Return either actual editor or editor fallback
    static std::optional<const EditorInfo *> TryGetEditor(entt::id_type Id);
    static std::optional<const EditorInfo *> TryGetEditor(const entt::type_info &Type);

    static EditorDrawFallback &GetEditorFallback();
    static EditorDrawFallback &GetEmptyEditor();

private:
};
}  // namespace Slipper::Editor
