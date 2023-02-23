#include "SceneObject.h"

#include "TransformComponent.h"

namespace Slipper
{
namespace SceneObject
{
Entity Create(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation)
{
    auto entity = Entity();
    entity.AddComponent<Transform>(Location, Scale, Rotation);

    return entity;
}
}  // namespace SceneObject
}  // namespace Slipper