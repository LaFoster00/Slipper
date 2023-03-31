#include "SceneObjectEditor.h"

namespace Slipper::Editor
{
void SceneObjectEditor::DrawEditor(entt::type_info Type, SceneObjectComponent &Component)
{
    auto temp = Component.GetName();
    temp.resize(MAX_SCENE_OBJECT_NAME_LENGTH - 1);
    if (ImGui::InputText("Name", temp.data(), temp.size()))
    {
        Component.SetName(temp);
    }
}
}  // namespace Slipper::Editor
