#pragma once

#include "EcsInterface.h"

namespace Slipper
{
struct Entity
{
    Entity() : m_entity()
    {
        m_entity = EcsInterface::CreateEntity();
    }

    Entity(entt::entity Entity)
    {
        m_entity = Entity;
    }

    Entity &operator=(const Entity &Other)
    {
        m_entity = Other.m_entity;
        return *this;
    }

    // Destroying Entities should ony be done explicitly since there is no reason for an entities
    // lifetime to be bound this wrapper
    void Destroy();

    template<typename T, typename... Args> decltype(auto) AddComponent(Args &&...args)
    {
        return EcsInterface::m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T> decltype(auto) GetComponent() const
    {
        return EcsInterface::m_registry.get<T>(m_entity);
    }

    template<typename T> std::optional<T &> TryGetComponent()
    {
        if (EcsInterface::m_registry.all_of<T>(m_entity))
            return std::optional<T>(GetComponent<T>());
        return std::optional<T>();
    }

    operator entt::entity &()
    {
        return m_entity;
    }

 private:
    entt::entity m_entity;
};
}  // namespace Slipper