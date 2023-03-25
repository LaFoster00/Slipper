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
    switch (Event.GetEventType()) {
        case EventType::MouseMoved:
            auto &cast_event = static_cast<MouseMovedEvent&>(Event);
            LOG_FORMAT("Event '{}' data: x{} y{}", Event.GetName(), cast_event.mouseX, cast_event.mouseY); 
            break;
    }
}
}  // namespace Slipper