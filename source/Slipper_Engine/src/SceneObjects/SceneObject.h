#pragma once

#include "Core/Entity.h"

namespace Slipper
{
namespace SceneObject
{
Entity Create(glm::vec3 Location = glm::vec3(0),
                         glm::vec3 Scale = glm::vec3(1),
                         glm::vec3 Rotation = glm::vec3(0));
}
}  // namespace Slipper