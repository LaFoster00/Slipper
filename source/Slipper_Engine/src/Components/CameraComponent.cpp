#include "CameraComponent.h"

#include "TransformComponent.h"

namespace Slipper
{
namespace Camera
{
glm::mat4 Parameters::GetProjection(float Aspect)
{
    auto projection = glm::perspective(glm::radians(fov), Aspect, nearPlane, farPlane);
    projection[1][1] *= -1;
    return projection;
}

void Parameters::UpdateViewTransform(Transform &Transform)
{
    view = glm::inverse(Transform.GetModelMatrix());
}
}  // namespace Camera
}  // namespace Slipper