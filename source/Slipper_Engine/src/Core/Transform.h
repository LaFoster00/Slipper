#pragma once

#include <functional>

#include "Engine.h"

namespace Slipper
{
using TransformChangeCallback = std::function<void()>;

struct Transform
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
    void SetRotation(glm::vec3 Rotation);
    glm::vec3 GetRotation() const;

    template<typename T> void SetScale(T Scale)
    {
        m_scale = glm::scale(glm::mat4(1.0f), glm::vec3(Scale));
        TransformChanged();
    }
    glm::vec3 GetScale();

    glm::mat4 GetModelMatrix() const
    {
        return m_location * glm::mat4(m_rotation) * m_scale;
    }

    void SetTransformChangeCallback(TransformChangeCallback Callback)
    {
        m_transformChangeCallback = Callback;
    }

    void LookAt(glm::vec3 Center, glm::vec3 Up = {0, 0, 1});

 private:
    void Create(glm::vec3 Location, glm::vec3 Scale, glm::vec3 Rotation);
    void TransformChanged() const
    {
        if (m_transformChangeCallback)
            m_transformChangeCallback();
    }

 private:
    glm::mat4 m_location;
    glm::mat4 m_scale;
    glm::quat m_rotation;

    TransformChangeCallback m_transformChangeCallback;
};
}  // namespace Slipper