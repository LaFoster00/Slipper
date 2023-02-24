#pragma once

#include "EcsInterface.h"

namespace Slipper
{
struct Entity
{
    Entity() : m_entity(), m_ecs(EcsInterface::Get())
    {
    }

    void Create()
    {
        m_entity = m_ecs.CreateEntity();
    }

    Entity(entt::entity Entity) : m_ecs(EcsInterface::Get())
    {
        m_entity = Entity;
    }

    Entity(const Entity &Other) : m_entity(Other.m_entity), m_ecs(Other.m_ecs)
    {
    }

    Entity &operator=(const Entity &Other)
    {
        m_entity = Other.m_entity;
        return *this;
    }

    // Destroying Entities should ony be done explicitly since there is no reason a entity should
    // be held explicitly
    void Destroy();

    template<typename T, typename... Args> void AddComponent(Args &&...args)
    {
        m_ecs.m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T> T &GetComponent()
    {
        return m_ecs.m_registry.get<T>(m_entity);
    }

    template<typename T> std::optional<T &> TryGetComponent()
    {
        if (m_ecs.m_registry.all_of<T>(m_entity))
            return std::optional<T>(GetComponent<T>());
        return std::optional<T>();
    }

 private:
    entt::entity m_entity;
    EcsInterface &m_ecs;
};
}  // namespace Slipper