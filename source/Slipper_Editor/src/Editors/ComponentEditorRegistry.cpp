#include "ComponentEditorRegistry.h"

namespace Slipper::Editor
{
EditorRegistry::EditorMap EditorRegistry::m_editors;
EditorRegistry::IdTypeToTypeInfoLookup EditorRegistry::m_idTypeTypeInfoLookup;
EditorRegistry::EditorDrawFallback EditorRegistry::m_fallback = [](const entt::type_info &Type) {
    bool open = true;
    ImGui::Begin("Fallback Editor", &open);
    ImGui::Text(
        "There is no editor registered for component of type: %s \n Consider adding one and "
        "registering it using the EditorRegistry::AddEditor() function!",
        Type.name().data());
    ImGui::End();
};

std::optional<EditorRegistry::EditorDrawFunc> EditorRegistry::TryGetEditor(entt::id_type Id)
{
    if (m_idTypeTypeInfoLookup.contains(Id))
        if (const auto type = m_idTypeTypeInfoLookup.at(Id); m_editors.contains(type)) {
            return m_editors.at(type);
        }
    return {};
}

std::optional<EditorRegistry::EditorDrawFunc> EditorRegistry::TryGetEditor(
    const entt::type_info &Type)
{
    if (m_editors.contains(&Type)) {
        return m_editors.at(&Type);
    }
    return {};
}

EditorRegistry::EditorDrawFallback &EditorRegistry::GetEditorFallback()
{
    return m_fallback;
}
}  // namespace Slipper::Editor
