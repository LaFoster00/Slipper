#include "Camera.h"

namespace Slipper
{
Camera::Camera(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation)
    : SlipperSceneObject(Location, Scale, Rotation)
{
    transform.SetTransformChangeCallback(std::bind(&Camera::OnTransformChange, this));
    OnTransformChange();
}

void Camera::OnInit()
{
}

void Camera::OnUpdate()
{
}

void Camera::OnDestroy()
{
}

glm::mat4 Camera::GetProjection(float Aspect) const
{
    auto projection = glm::perspective(glm::radians(m_fov), Aspect, m_nearPlane, m_farPlane);
    projection[1][1] *= -1;
    return projection;
}

void Camera::UpdateViewTransform()
{
    m_view = glm::inverse(transform.GetModelMatrix());
}

void Camera::OnTransformChange()
{
    UpdateViewTransform();
}
}  // namespace Slipper