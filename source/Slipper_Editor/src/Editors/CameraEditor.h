#pragma once

#include "CameraComponent.h"
#include "IComponentEditor.h"

namespace Slipper::Editor
{
struct CameraEditor : public IComponentEditor<CameraEditor, Camera>
{
    void DrawEditor(entt::type_info Type, Camera &Component) override;
    uint32_t Impl_GetLines() override;
    std::string_view Impl_GetName() override;
};
}  // namespace Slipper::Editor
