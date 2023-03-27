#include "EditorCameraSystem.h"

#include "CameraComponent.h"
#include "TransformComponent.h"

namespace Slipper::Editor
{
void EditorCameraSystem::Execute(entt::registry &Registry)
{
    auto view = Registry.view<Transform, Camera::Parameters, EditorCameraComponent>();
    for (auto entity : view) {
        auto &transform = view.get<Transform>(entity);
        auto &camera_params = view.get<Camera::Parameters>(entity);
        
    }
}
}  // namespace Slipper::Editor
