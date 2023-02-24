#pragma once

#include "entt/entt.hpp"

namespace Slipper
{
class EcsInterface
{
    friend struct Entity;
 public:
    static void Create();
    static void Destroy();

    static EcsInterface &Get()
    {
        return *m_instance;
    }

public:
    entt::registry &GetRegistry()
    {
        return m_registry;
    }

 private:
    entt::entity CreateEntity();

 private:
    static EcsInterface *m_instance;
    entt::registry m_registry;
};
}  // namespace Slipper