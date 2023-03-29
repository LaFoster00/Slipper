#include "CameraComponent.h"

#include "TransformComponent.h"

namespace Slipper
{
glm::mat4 Camera::GetProjection(float Aspect)
{
    auto projection = glm::perspective(glm::radians(fov), Aspect, nearPlane, farPlane);
    projection[1][1] *= -1;
    return projection;
}

glm::mat4 Camera::GetView()
{
    return view;
}

std::tuple<glm::mat4, glm::mat4> Camera::GetViewProjection(float Aspect, Transform &Transform)
{
    UpdateViewTransform(Transform);
    return {GetView(), GetProjection(Aspect)};
}

std::tuple<glm::mat4, glm::mat4> Camera::GetViewProjection(Entity Entity, float Aspect)
{
    auto &camera_transform = Entity.GetComponent<Transform>();
    auto &camera_parameters = Entity.GetComponent<Camera>();

    return camera_parameters.GetViewProjection(Aspect, camera_transform);
}

void Camera::UpdateViewTransform(Transform &Transform)
{
    view = glm::inverse(Transform.GetModelMatrix());
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