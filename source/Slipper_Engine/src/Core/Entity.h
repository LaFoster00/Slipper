#pragma once

#include "EcsInterface.h"

namespace Slipper
{
struct Entity
{
    using ComponentTypeLookup = std::map<const entt::id_type, const entt::type_info *>;

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
        //Should be called for every type since template gets instantiated for every type
        static const bool TypeInfoGenerated = InitComponentLookup<T>();
        m_ecs.m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    }

    template<typename T>
    bool InitComponentLookup()
    {
        const entt::type_info &typeInfo = entt::type_id<T>();
        const entt::id_type id = typeInfo.hash();
        m_componentTypeLookup[id] = &typeInfo;
        return true;
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

	operator entt::entity &()
    {
        return m_entity;
    }

    static const entt::type_info &GetComponentType(const entt::id_type Id)
    {
        return *m_componentTypeLookup.at(Id);
    }

 private:
    entt::entity m_entity;
    EcsInterface &m_ecs;

    static ComponentTypeLookup m_componentTypeLookup;
};
}  // namespace Slipper