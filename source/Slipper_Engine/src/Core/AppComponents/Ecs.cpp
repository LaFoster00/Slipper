#include "Ecs.h"

#include "Core/EcsInterface.h"
#include "Core/Event.h"

namespace Slipper
{
void Ecs::Init()
{
}

void Ecs::Shutdown()
{

}

void Ecs::OnUpdate()
{
    EcsInterface::RunSystems();
}

void Ecs::OnEvent(Event &Event)
{

}

void Ecs::OnGuiRender()
{

}
}  // namespace Slipper