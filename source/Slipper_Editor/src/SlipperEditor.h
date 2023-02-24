#pragma once
#include "Core/Application.h"

namespace Slipper::Editor
{
class SlipperEditor : public Slipper::Application
{
 public:
    using Application::Application;

    void Init() override;
};
}  // namespace Slipper::Editor
