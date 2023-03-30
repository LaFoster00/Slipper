#pragma once

#include "SceneObject.h"

namespace Slipper
{
extern Entity CreateCamera(std::string_view Name = "SceneObject",
                           glm::vec3 Location = glm::vec3(2, 2, 2),
                           glm::vec3 Scale = glm::vec3(1),
                           glm::vec3 Rotation = glm::vec3(60.0f, 0.0f, 135.0f));
}  // namespace Slipper
