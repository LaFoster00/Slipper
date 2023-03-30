#include "SceneObject.h"

#include "SceneObjectComponent.h"
#include "TransformComponent.h"

namespace Slipper
{
namespace SceneObject
{
Entity Create(std::string_view Name,
              glm::vec3 Location,
              glm::vec3 Scale,
              glm::vec3 Rotation)
{
    auto entity = Entity();
    entity.AddComponent<SceneObjectComponent>(Name);
    entity.AddComponent<Transform>(Location, Scale, Rotation);

    return entity;
}
}  // namespace SceneObject
}  // namespace Slipper