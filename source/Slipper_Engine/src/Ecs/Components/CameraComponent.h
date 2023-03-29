#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
struct Transform;

struct Camera : public IEcsComponent<Camera>
{
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 10.0f;
    // Diagonal(mm); default : Sony A7 II 43.04mm
    float sensorSize = 43.04;
    glm::mat4 view = {};

    // Aspect is Width / Height
    [[nodiscard]] glm::mat4 GetProjection(float Aspect);
    [[nodiscard]] glm::mat4 GetView();
    [[nodiscard]] std::tuple<glm::mat4, glm::mat4> GetViewProjection(float Aspect,
                                                                     Transform &Transform);
    [[nodiscard]] static std::tuple<glm::mat4, glm::mat4> GetViewProjection(Entity Camera,
        float Aspect);
    void UpdateViewTransform(Transform &Transform);

    void SetFov(float Fov);
    // Full frame camera
    void SetFocalLength(float FocalLength);
    float GetFov() const;
    // Full frame camera
    float GetFocalLength() const;
};
}  // namespace Slipper