#pragma once

#include "entt/entt.hpp"

namespace Slipper
{
struct EcsSystemData;

class EcsInterface
{
    friend struct Entity;

 public:
    struct ComponentTypeInfo
    {
        const entt::type_info &typeInfo;
        const size_t size;
    };

    using ComponentTypeLookup = std::map<const entt::id_type, ComponentTypeInfo>;

 public:
    static bool AddSystem(EcsSystemData &Data);
    static void RunSystems();

    static entt::registry &GetRegistry()
    {
        return m_registry;
    }

    template<typename T> static bool InitComponentLookup()
    {
        const entt::type_info &typeInfo = entt::type_id<T>();
        const entt::id_type id = typeInfo.hash();
        if (!m_componentTypeLookup.contains(id)) {
            m_componentTypeLookup.emplace(id, ComponentTypeInfo{typeInfo, sizeof(T)});
        }
        return true;
    }

    static ComponentTypeInfo &GetComponentType(const entt::id_type Id)
    {
        return m_componentTypeLookup.at(Id);
    }

 private:
    static entt::entity CreateEntity();

 private:
    static inline entt::registry m_registry = entt::registry();
    static inline std::vector<Ref<EcsSystemData>> m_ecsSystems;

    static inline ComponentTypeLookup m_componentTypeLookup;
};
}  // namespace Slipper