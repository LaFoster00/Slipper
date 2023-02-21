#pragma once
#include "Core/Application.h"

class SlipperEngineTest : public Slipper::Application
{
 public:
    using Application::Application;

    void Init() override;
};
