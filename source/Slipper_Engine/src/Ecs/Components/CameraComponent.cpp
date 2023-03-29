#include "CameraComponent.h"

#include "TransformComponent.h"

namespace Slipper
{
glm::mat4 Camera::GetProjection(float Aspect, bool OpenGlStyle) const
{
    auto projection = glm::perspective(glm::radians(fov), Aspect, nearPlane, farPlane);
    if (!OpenGlStyle)
        projection[1][1] *= -1;
    return projection;
}

glm::mat4 Camera::GetView() const
{
    const auto &transform = entity.GetComponent<Transform>();
    return glm::inverse(transform.GetModelMatrix());
}

glm::mat4 Camera::GetView(const Transform &Transform) const
{
    return glm::inverse(Transform.GetModelMatrix());
}

std::tuple<glm::mat4, glm::mat4> Camera::GetViewProjection(float Aspect, bool OpenGlStyle) const
{
    return {GetView(), GetProjection(Aspect, OpenGlStyle)};
}

std::tuple<glm::mat4, glm::mat4> Camera::GetViewProjection(Entity Entity, float Aspect)
{
    const auto &camera_parameters = Entity.GetComponent<Camera>();

    return camera_parameters.GetViewProjection(Aspect);
}

void Camera::SetFov(const float Fov)
{
    fov = Fov;
}

void Camera::SetFocalLength(const float FocalLength)
{
    fov = 2 * atan(sensorSize / (2 * FocalLength)) * 180 / glm::pi<float>();
}

float Camera::GetFov() const
{
    return fov;
}

float Camera::GetFocalLength() const
{
    return sensorSize / (2 * tan((fov * glm::pi<float>() / 180) / 2));
}
}  // namespace Slipper