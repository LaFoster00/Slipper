#include "Camera.h"

namespace Slipper
{
namespace Camera
{
Entity Create(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation)
{
    auto camera = SceneObject::Create(Location, Scale, Rotation);
    camera.AddComponent<Parameters>();
    return camera;
}
}  // namespace Camera
}  // namespace Slipper