#pragma once

namespace Slipper
{
struct Transform;

namespace Camera
{
struct Parameters
{
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 10.0f;
    glm::mat4 view = {};

    // Aspect is Width / Height
    [[nodiscard]] glm::mat4 GetProjection(float Aspect);

    void UpdateViewTransform(Transform &Transform);
};
}  // namespace Camera
}  // namespace Slipper