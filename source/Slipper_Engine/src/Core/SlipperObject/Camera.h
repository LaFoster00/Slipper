#pragma once
#include "Core/SlipperSceneObject.h"
#include "Engine.h"

namespace Slipper
{
class Camera : public SlipperSceneObject
{
 public:
    Camera(glm::vec3 Location = glm::vec3(2, 2, 2),
           glm::vec3 Scale = glm::vec3(1),
           glm::vec3 Rotation = glm::vec3(60.0f, 0.0f, 135.0f));
    void OnInit() override;
    void OnUpdate() override;
    void OnDestroy() override;

    void SetFieldOfView(const float Fov)
    {
        m_fov = Fov;
    }
    float GetFieldOfView() const
    {
        return m_fov;
    }

    void SetNearPlane(const float Np)
    {
        m_nearPlane = Np;
    }
    float GetNearPlane() const
    {
        return m_nearPlane;
    }

    void SetFarPlane(const float Fp)
    {
        m_farPlane = Fp;
    }
    float GetFarPlane() const
    {
        return m_farPlane;
    }

    [[nodiscard]] glm::mat4 GetView() const
    {
        return m_view;
    }

    // Aspect is Width / Height
    [[nodiscard]] glm::mat4 GetProjection(float Aspect) const;

 private:
    void UpdateViewTransform();
    void OnTransformChange();

 private:
    glm::mat4 m_view = {};

    float m_fov = 45.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 10.0f;
};
}  // namespace Slipper
