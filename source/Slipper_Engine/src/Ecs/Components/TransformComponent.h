#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
using TransformChangeCallback = std::function<void()>;

struct Transform : public IEcsComponent
{
    Transform(glm::vec3 Location = glm::vec3(0),
              glm::vec3 Scale = glm::vec3(1),
              glm::vec3 Rotation = glm::vec3(0))
    {
        Create(Location, Scale, Rotation);
    }

    void SetLocation(glm::vec3 Location);
    glm::vec3 GetLocation();
    void Translate(glm::vec3 Translation);
    void Rotate(glm::vec3 Rotation);
    void Rotate(float Angle, glm::vec3 Axis);
    void SetRotation(glm::vec3 Rotation);
    glm::vec3 GetRotation() const;

    template<typename T> void SetScale(T Scale)
    {
        m_scale = glm::scale(glm::mat4(1.0f), glm::vec3(Scale));
    }
    glm::vec3 GetScale();

    glm::mat4 GetModelMatrix() const
    {
        return m_location * glm::mat4(m_rotation) * m_scale;
    }

    void LookAt(glm::vec3 Center, glm::vec3 Up = {0, 0, 1});

    glm::vec3 Forward() const
    {
        return m_rotation * glm::vec3(1, 0, 0);
    }

    glm::vec3 Up() const
    {
        return m_rotation * glm::vec3(0, 0, 1);
    }

    glm::vec3 Right() const
    {
        return m_rotation * glm::vec3(0, 1, 0);
    }

 private:
    void Create(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation);

 private:
    glm::mat4 m_location;
    glm::mat4 m_scale;
    glm::quat m_rotation;
};
}  // namespace Slipper