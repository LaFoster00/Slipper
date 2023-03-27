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

glm::mat4 Parameters::GetView()
{
    return view;
}

std::tuple<glm::mat4, glm::mat4> Parameters::GetViewProjection(float Aspect, Transform &Transform)
{
    UpdateViewTransform(Transform);
    return {GetView(), GetProjection(Aspect)};
}

std::tuple<glm::mat4, glm::mat4> Parameters::GetViewProjection(Entity& Camera, float Aspect)
{
    auto &camera_transform = Camera.GetComponent<Transform>();
    auto &camera_parameters = Camera.GetComponent<Parameters>();

    return camera_parameters.GetViewProjection(Aspect, camera_transform);
}

void Parameters::UpdateViewTransform(Transform &Transform)
{
    view = glm::inverse(Transform.GetModelMatrix());
}

void Parameters::SetFov(const float Fov)
{
    fov = Fov;
}

void Parameters::SetFocalLength(const float FocalLength)
{
    fov = 2 * atan(sensorSize / (2 * FocalLength)) * 180 / glm::pi<float>();
}

float Parameters::GetFov() const
{
    return fov;
}

float Parameters::GetFocalLength() const
{
    return sensorSize / (2 * tan((fov * glm::pi<float>() / 180) / 2));
}
}  // namespace Camera
}  // namespace Slipper