#include "EditorCameraSystem.h"

#include "CameraComponent.h"
#include "Input.h"
#include "Time/Time.h"
#include "TransformComponent.h"

namespace Slipper::Editor
{
void EditorCameraSystem::Execute(entt::registry &Registry)
{
    auto view = Registry.view<Transform, Camera, EditorCameraComponent>();
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
            auto &camera_params = view.get<Camera>(entity);

            auto view_rotation = Input::GetMouseMovement() * 100.0f * Time::DeltaTime();
            transform.Rotate(-view_rotation.x, {0, 0, 1});
            transform.Rotate(-view_rotation.y, transform.Forward());

            const float MovementSpeed = 10.0f;
            glm::vec3 translation{0.0f, 0.0f, 0.0f};

            if (Input::GetKeyDown(KeyCode::W)) {
                translation += -transform.Up();
            }
            if (Input::GetKeyDown(KeyCode::S)) {
                translation += transform.Up();
            }
            if (Input::GetKeyDown(KeyCode::A)) {
                translation += -transform.Forward();
            }
            if (Input::GetKeyDown(KeyCode::D)) {
                translation += transform.Forward();
            }
            if (Input::GetKeyDown(KeyCode::Q)) {
                translation += -transform.Right();
            }
            if (Input::GetKeyDown(KeyCode::E)) {
                translation += transform.Right();
            }

            if (translation != glm::vec3{0.0f, 0.0f, 0.0f})
                translation = glm::normalize(translation);
            transform.Translate(translation * Time::DeltaTime() * MovementSpeed);
        }
    }
}
}  // namespace Slipper::Editor
