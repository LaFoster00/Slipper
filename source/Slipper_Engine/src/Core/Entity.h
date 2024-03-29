#pragma once

#include "EcsInterface.h"
#include "IEcsComponent.h"

namespace Slipper
{
struct Entity
{
    Entity() : m_entity()
    {
        m_entity = EcsInterface::CreateEntity();
    }

    Entity(entt::null_t)
    {
        m_entity = entt::null;
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

    operator bool() const
    {
        return IsValid();
    }

    bool IsValid() const
    {
        return !(m_entity == entt::null || !EcsInterface::m_registry.valid(m_entity));
    }

    static Entity InvalidEntity()
    {
        return Entity(entt::null);
    }

    // Destroying Entities should ony be done explicitly since there is no reason for an entities
    // lifetime to be bound this wrapper
    void Destroy();

    template<typename T, typename... Args>
        requires(std::is_base_of_v<IEcsComponent<T>, T> &&
                 !std::is_base_of_v<IEcsComponentWithEntity<T>, T>) decltype(auto)
    AddComponent(Args &&...args)
    {
        return EcsInterface::m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
        requires(std::is_base_of_v<IEcsComponent<T>, T> &&
                 std::is_base_of_v<IEcsComponentWithEntity<T>, T>) decltype(auto)
    AddComponent(Args &&...args)
    {
        return EcsInterface::m_registry.emplace<T>(m_entity, *this, std::forward<Args>(args)...);
    }

    template<typename T>
        requires std::is_base_of_v<IEcsComponent<T>, T> decltype(auto)
    GetComponent() const
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

    auto operator<=>(const Entity &) const = default;

 private:
    entt::entity m_entity;
};
}  // namespace Slipper