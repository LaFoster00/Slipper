#include "Entity.h"

namespace Slipper
{
void Entity::Destroy()
{
    m_ecs.m_registry.destroy(m_entity);
}
}  // namespace Slipper
