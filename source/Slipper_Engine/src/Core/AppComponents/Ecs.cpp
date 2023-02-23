#include "Ecs.h"

#include "Core/EcsInterface.h"

namespace Slipper
{
void Ecs::Init()
{
    EcsInterface::Create();
}

void Ecs::Shutdown()
{
    EcsInterface::Destroy();
}

void Ecs::OnUpdate()
{
}
}  // namespace Slipper
