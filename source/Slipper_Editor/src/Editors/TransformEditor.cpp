#include "TransformEditor.h"

namespace Slipper::Editor
{
void TransformEditor::DrawEditor(entt::type_info Type, Transform &Component)
{
    auto location = Component.GetLocation();
    auto rotation = Component.GetRotation();
    auto scale = Component.GetScale();
    ImGui::DragFloat3("Location", &location.x, 0.01f);
    ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
    ImGui::DragFloat3("Scale", &scale.x, 0.01f);
    Component.SetLocation(location);
    Component.SetRotation(rotation);
    Component.SetScale(scale);
}

uint32_t TransformEditor::Impl_GetLines()
{
    return 3;
}

std::string_view TransformEditor::Impl_GetName()
{
    return "Transform";
}
}  // namespace Slipper::Editor
