#pragma once

#include "entt/entt.hpp"

namespace Slipper
{
struct EcsSystemData;

class EcsInterface
{
    friend struct Entity;

 public:
    EcsInterface();

    static bool AddSystem(EcsSystemData &Data);
    static void RunSystems();

    static EcsInterface &Get()
    {
        static EcsInterface instance;
        return instance;
    }

 public:
    entt::registry &GetRegistry()
    {
        return m_registry;
    }

 private:
    entt::entity CreateEntity();

 private:
    entt::registry m_registry;
    std::vector<Ref<EcsSystemData>> m_ecsSystems;
};
}  // namespace Slipper