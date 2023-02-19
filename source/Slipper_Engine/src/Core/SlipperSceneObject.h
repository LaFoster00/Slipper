#pragma once
#include "SlipperObject.h"
#include "Transform.h"

namespace Slipper
{
class SlipperSceneObject : public SlipperObject
{
 public:
    SlipperSceneObject(glm::vec3 Location = glm::vec3(0),
                       glm::vec3 Scale = glm::vec3(1),
                       glm::vec3 Rotation = glm::vec3(0));

 public:
    Transform transform;
};
}  // namespace Slipper