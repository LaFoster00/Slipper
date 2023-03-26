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
	AppComponent::OnGuiRender();

    bool open = true;
    ImGui::SetNextWindowSize({300, 300});
    ImGui::Begin("Hello World", &open);
    if (ImGui::IsItemHovered()) {
        LOG_FORMAT("HELLO {}", Engine::FRAME_COUNT);
    }
    ImGui::End();
}
}  // namespace Slipper