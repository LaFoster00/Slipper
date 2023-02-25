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
    // Diagonal(mm); default : Sony A7 II 43.04mm
    float sensorSize = 43.04;
    glm::mat4 view = {};

    // Aspect is Width / Height
    [[nodiscard]] glm::mat4 GetProjection(float Aspect);

    void UpdateViewTransform(Transform &Transform);

    void SetFov(float Fov);
    // Full frame camera
    void SetFocalLength(float FocalLength);
    float GetFov() const;
    // Full frame camera
    float GetFocalLength() const;
};
}  // namespace Camera
}  // namespace Slipper