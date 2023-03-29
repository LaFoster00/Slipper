#include "Entity.h"

namespace Slipper
{

void Entity::Destroy()
{
    EcsInterface::m_registry.destroy(m_entity);
}
}  // namespace Slipper
