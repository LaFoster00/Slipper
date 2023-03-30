#include "SceneObjectEditor.h"

namespace Slipper::Editor
{
void SceneObjectEditor::DrawEditor(entt::type_info Type, SceneObjectComponent &Component)
{
    ImGui::Text(std::format("\t{}", Component.name.data()).c_str());
}
}  // namespace Slipper::Editor
