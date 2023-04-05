#include "CameraEditor.h"

namespace Slipper::Editor
{
void CameraEditor::DrawEditor(entt::type_info Type, Camera &Component)
{
    ImGui::DragFloat("Far Plane", &Component.farPlane, 0.1f, 0.01f);
    ImGui::DragFloat("Near Plane", &Component.nearPlane, 0.1f, 0.01f);

    {
        float fov = Component.GetFov();
        if (ImGui::DragFloat("Fov", &fov, 0.1f, 5.0f, 179.0f))
            Component.SetFov(fov);
    }
    {
        float focal_length = Component.GetFocalLength();
        {
            if (ImGui::DragFloat("Sensor Diagonal (mm)", &Component.sensorSize, 0.1f, 1.0f))
                Component.SetFocalLength(focal_length);
        }
        {
            if (ImGui::DragFloat("Focal Length (mm)", &focal_length, 0.1f, 1.0f))
                Component.SetFocalLength(focal_length);
        }
    }
}

std::string_view CameraEditor::Impl_GetName()
{
    return "Camera";
}
}  // namespace Slipper::Editor
