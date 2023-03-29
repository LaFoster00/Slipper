#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
using TransformChangeCallback = std::function<void()>;

struct Transform : public IEcsComponent<Transform>
{
    Transform(glm::vec3 Location = glm::vec3(0),
              glm::vec3 Scale = glm::vec3(1),
              glm::vec3 Rotation = glm::vec3(0))
    {
        SetLocation(Location);
        SetScale(Scale);
        SetRotation(Rotation);
    }

    void SetLocation(glm::vec3 Location)
    {
        m_location = Location;
    }

    glm::vec3 GetLocation()
    {
        return m_location;
    }

    void Translate(glm::vec3 Translation)
    {
        m_location += Translation;
    }

    void LocalTranslate(glm::vec3 Translation)
    {
        m_location += glm::inverse(m_rotation) * Translation;
    }

    void Rotate(glm::vec3 Rotation)
    {
        m_rotation = glm::quat(glm::radians(Rotation)) * m_rotation;
    }

    void Rotate(float Angle, glm::vec3 Axis)
    {
        m_rotation = glm::rotate(glm::quat({0, 0, 0}), glm::radians(Angle), Axis) * m_rotation;
    }

    void RotateLocal(float Angle, glm::vec3 Axis)
    {
        m_rotation = glm::rotate(m_rotation, glm::radians(Angle), glm::inverse(m_rotation) * Axis);
    }

    void LocalRotate(glm::vec3 Rotation)
    {
        m_rotation *= glm::quat(glm::radians(Rotation)) * m_rotation;
    }

    void SetRotation(glm::vec3 Rotation)
    {
        m_rotation = glm::quat(glm::radians(Rotation));
    }

    glm::vec3 GetRotation() const
    {
        return glm::degrees(glm::eulerAngles(m_rotation));
    }

    template<typename T> void SetScale(T Scale)
    {
        m_scale = Scale;
    }

    glm::vec3 GetScale() const
    {
        return m_scale;
    }

    glm::mat4 GetModelMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), m_location) * glm::mat4_cast(m_rotation) *
               glm::scale(glm::mat4(1.0f), m_scale);
    }

    void LookAt(glm::vec3 Center, glm::vec3 Up = {0, 0, 1})
    {
        const auto dir = glm::normalize(Center - GetLocation());
        m_rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(dir, Up)));
    }

    glm::vec3 Forward() const
    {
        return m_rotation * glm::vec3(1, 0, 0);
    }

    glm::vec3 Right() const
    {
        return m_rotation * glm::vec3(0, 1, 0);
    }

    glm::vec3 Up() const
    {
        return m_rotation * glm::vec3(0, 0, 1);
    }

 private:
    glm::vec3 m_location = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({0, 0, 0});
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};
}  // namespace Slipper