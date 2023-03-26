#include "Ecs.h"

#include "Core/EcsInterface.h"
#include "Core/Event.h"
#include "Core/InputEvent.h"

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

void Ecs::OnEvent(Event &Event)
{

}

void Ecs::OnGuiRender()
{

}
}  // namespace Slipper