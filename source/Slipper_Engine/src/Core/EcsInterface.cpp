#include "EcsInterface.h"

namespace Slipper
{
EcsInterface *EcsInterface::m_instance = nullptr;

void EcsInterface::Create()
{
    if (!m_instance)
        m_instance = new EcsInterface();

    m_instance->m_registry = entt::registry();
}

void EcsInterface::Destroy()
{
    delete m_instance;
}

entt::entity EcsInterface::CreateEntity()
{
    return m_registry.create();
}
}  // namespace Slipper