#pragma once
#include "Core/AppComponent.h"

class TestGui : public Slipper::AppComponent
{
public:
    void OnGuiRender() override;
};