#include "EcsInterface.h"

#include "IEcsSystem.h"

namespace Slipper
{
EcsInterface::EcsInterface()
{
    m_registry = entt::registry();
}

bool EcsInterface::AddSystem(EcsSystemData &Data)
{
    Get().m_ecsSystems.push_back(Data);
    return true;
}

void EcsInterface::RunSystems()
{
    auto &registry = Get().m_registry;
    for (const auto &ecs_system : Get().m_ecsSystems) {
        if (ecs_system.get().executeFunction) {
            ecs_system.get().executeFunction(registry);
        }
    }
}

entt::entity EcsInterface::CreateEntity()
{
    return m_registry.create();
}
}  // namespace Slipper