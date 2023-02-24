#include "Entity.h"

namespace Slipper
{
Entity::ComponentTypeLookup Entity::m_componentTypeLookup;

void Entity::Destroy()
{
    m_ecs.m_registry.destroy(m_entity);
}
}  // namespace Slipper
