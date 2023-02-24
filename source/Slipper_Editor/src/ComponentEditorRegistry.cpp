#include "ComponentEditorRegistry.h"

namespace Slipper::Editor
{
EditorRegistry::EditorMap EditorRegistry::m_editors;
std::function<void(entt::type_info)> EditorRegistry::m_fallback = [](entt::type_info Type) {
    bool open = true;
    ImGui::Begin("Fallback Editor", &open);
    ImGui::Text("There is no editor registered for component of type %s: ", Type.name());
    ImGui::End();
};

std::optional<EditorRegistry::EditorDrawFunc> EditorRegistry::GetEditor(entt::type_info Type)
{
    if (m_editors.contains(Type)) {
        return m_editors.at(Type);
    }
    return {};
}

EditorRegistry::EditorDrawFallback& EditorRegistry::GetEditorFallback()
{
    return m_fallback;
}
}  // namespace Slipper::Editor
