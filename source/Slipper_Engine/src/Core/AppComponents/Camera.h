#pragma once
#include "Core/AppComponent.h"

namespace Slipper
{
class Camera : AppComponent
{
public:
    void Init() override;
    void Shutdown() override;
    void OnUpdate() override;
    void OnGuiRender() override;
};
}
