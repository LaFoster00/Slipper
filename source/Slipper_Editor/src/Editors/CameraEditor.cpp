#include "CameraEditor.h"

namespace Slipper::Editor
{
void CameraEditor::DrawEditor(entt::type_info Type, Camera::Parameters &Component)
{
    bool open = true;
    ImGui::Begin("Editor Bitch", &open);
    ImGui::Text("Camera iz da");
    ImGui::End();
}
}