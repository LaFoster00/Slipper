#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
struct Transform;

struct Camera : public IEcsComponentWithEntity<Camera>
{
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 10.0f;
    // Diagonal(mm); default : Sony A7 II 43.04mm
    float sensorSize = 43.04;

    // Aspect is Width / Height
    [[nodiscard]] glm::mat4 GetProjection(float Aspect, bool OpenGlStyle = false) const;
    [[nodiscard]] glm::mat4 GetView() const;
    [[nodiscard]] glm::mat4 GetView(const Transform &Transform) const;
    [[nodiscard]] std::tuple<glm::mat4, glm::mat4> GetViewProjection(
        float Aspect, bool OpenGlStyle = false) const;
    [[nodiscard]] static std::tuple<glm::mat4, glm::mat4> GetViewProjection(Entity Entity,
        float Aspect);

    void SetFov(float Fov);
    // Full frame camera
    void SetFocalLength(float FocalLength);
    float GetFov() const;
    // Full frame camera
    float GetFocalLength() const;
};
}  // namespace Slipper