#include "EditorCameraSystem.h"

#include "CameraComponent.h"
#include "Core/Input.h"
#include "Time/Time.h"
#include "TransformComponent.h"

namespace Slipper::Editor
{
void EditorCameraSystem::Execute(entt::registry &Registry)
{
    auto view = Registry.view<Transform, Camera::Parameters, EditorCameraComponent>();
    for (auto entity : view) {

        // transform.Translate(-transform.Up() * Time::DeltaTime());

        if (Input::GetMouseButtonPressed(MouseCode::ButtonRight)) {
            Input::CaptureMouse(true);
        }
        else if (Input::GetMouseButtonReleased(MouseCode::ButtonRight)) {
            Input::CaptureMouse(false);
        }

        if (Input::GetMouseButtonDown(MouseCode::ButtonRight)) {
            auto &transform = view.get<Transform>(entity);
            auto &camera_params = view.get<Camera::Parameters>(entity);

            auto view_rotation = Input::GetMouseMovement() * 100.0f * Time::DeltaTime();
            transform.Rotate(-view_rotation.x, {0, 0, 1});
            transform.Rotate(-view_rotation.y, transform.Forward());

            const float MovementSpeed = 10.0f;

            if (Input::GetKeyDown(KeyCode::W)) {
                transform.Translate(-transform.Up() * Time::DeltaTime() * MovementSpeed);
            }
            if (Input::GetKeyDown(KeyCode::S)) {
                transform.Translate(transform.Up() * Time::DeltaTime() * MovementSpeed);
            }
            if (Input::GetKeyDown(KeyCode::A)) {
                transform.Translate(-transform.Forward() * Time::DeltaTime() * MovementSpeed);
            }
            if (Input::GetKeyDown(KeyCode::D)) {
                transform.Translate(transform.Forward() * Time::DeltaTime() * MovementSpeed);
            }
            if (Input::GetKeyDown(KeyCode::Q)) {
                transform.Translate(-transform.Right() * Time::DeltaTime() * MovementSpeed);
            }
            if (Input::GetKeyDown(KeyCode::E)) {
                transform.Translate(transform.Right() * Time::DeltaTime() * MovementSpeed);
            }
        }
    }
}
}  // namespace Slipper::Editor
