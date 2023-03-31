#include "EcsInterface.h"

namespace Slipper
{
void EcsInterface::RunSystems()
{
    auto &registry = m_registry;
    for (const auto &ecs_system : m_ecsSystems) {
        if (ecs_system.executeFunction) {
            ecs_system.executeFunction(registry);
        }
    }
}

entt::entity EcsInterface::CreateEntity()
{
    return m_registry.create();
}
}  // namespace Slipper