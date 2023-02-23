#pragma once

#include "SceneObject.h"
#include "CameraComponent.h"

namespace Slipper
{
namespace Camera
{
Entity Create(glm::vec3 Location = glm::vec3(2, 2, 2),
                    glm::vec3 Scale = glm::vec3(1),
                    glm::vec3 Rotation = glm::vec3(60.0f, 0.0f, 135.0f));
};  // namespace Camera
}  // namespace Slipper
