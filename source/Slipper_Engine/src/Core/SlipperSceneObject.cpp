#include "SlipperSceneObject.h"

namespace Slipper
{
SlipperSceneObject::SlipperSceneObject(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation)
    : transform(Location, Scale, Rotation)
{
}
}  // namespace Slipper