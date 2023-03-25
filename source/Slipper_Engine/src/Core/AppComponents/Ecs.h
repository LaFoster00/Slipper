#pragma once

#include "Core/AppComponent.h"

namespace Slipper
{
class EcsInterface;

class Ecs : public AppComponent
{
 public:
    Ecs() : AppComponent("Ecs")
    {
    }
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnEvent(Event& Event) override;

    public:
    OwningPtr<EcsInterface> interface;
};
}  // namespace Slipper