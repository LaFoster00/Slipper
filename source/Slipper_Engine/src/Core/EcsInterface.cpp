#include "EcsInterface.h"

#include "IEcsSystem.h"

namespace Slipper
{
bool EcsInterface::AddSystem(EcsSystemData &Data)
{
    m_ecsSystems.push_back(Data);
    return true;
}

void EcsInterface::RunSystems()
{
    auto &registry = m_registry;
    for (const auto &ecs_system : m_ecsSystems) {
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