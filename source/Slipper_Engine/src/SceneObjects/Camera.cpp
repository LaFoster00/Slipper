#include "Camera.h"

#include "CameraComponent.h"

namespace Slipper
{
    Entity CreateCamera(std::string_view Name, glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation)
    {
        auto camera = SceneObject::Create(Name, Location, Scale, Rotation);
        camera.AddComponent<Camera>();
        return camera;
    }
}  // namespace Slipper
