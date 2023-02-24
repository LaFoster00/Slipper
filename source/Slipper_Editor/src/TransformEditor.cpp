#include "TransformEditor.h"

namespace Slipper::Editor
{
void TransformEditor::DrawEditor(type_info Type, Transform &Component)
{
    bool open = true;
    ImGui::Begin("Editor Bitch", &open);
    ImGui::Text("Transform iz da");
    ImGui::End();
}
}  // namespace Slipper::Editor
