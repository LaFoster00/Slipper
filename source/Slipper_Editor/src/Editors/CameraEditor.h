#pragma once

#include "CameraComponent.h"
#include "IComponentEditor.h"

namespace Slipper::Editor
{
struct CameraEditor : public IComponentEditor<CameraEditor, Camera::Parameters>
{
    void DrawEditor(entt::type_info Type, Camera::Parameters &Component) override;
};
}  // namespace Slipper::Editor
