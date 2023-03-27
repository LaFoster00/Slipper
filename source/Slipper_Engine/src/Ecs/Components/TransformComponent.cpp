#include "TransformComponent.h"

namespace Slipper
{
void Transform::SetLocation(const glm::vec3 Location)
{
    m_location = glm::translate(glm::mat4(1.0f), Location);
}

glm::vec3 Transform::GetLocation()
{
    return glm::vec3(m_location[3]);
}

void Transform::Translate(const glm::vec3 Translation)
{
    m_location = glm::translate(m_location, Translation);
}

void Transform::Rotate(const glm::vec3 Rotation)
{
    m_rotation = glm::quat(glm::radians(Rotation)) * m_rotation;
}

void Transform::SetRotation(const glm::vec3 Rotation)
{
    m_rotation = glm::quat(glm::radians(Rotation));
}

glm::vec3 Transform::GetRotation() const
{
    return glm::degrees(glm::eulerAngles(m_rotation));
}

glm::vec3 Transform::GetScale()
{
    return {glm::length(m_scale[0]), glm::length(m_scale[1]), glm::length(m_scale[2])};
}

void Transform::LookAt(glm::vec3 Center, glm::vec3 Up)
{
    const auto dir = glm::normalize(Center - GetLocation());
    m_rotation = glm::quatLookAt(dir, Up);
}

void Transform::Create(const glm::vec3 Location, const glm::vec3 Scale, const glm::vec3 Rotation)
{
    SetLocation(Location);
    SetScale(Scale);
    SetRotation(Rotation);
}
}  // namespace Slipper
