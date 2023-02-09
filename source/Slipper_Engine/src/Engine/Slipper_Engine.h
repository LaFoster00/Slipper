#pragma once
#include "Core/Application.h"

namespace Slipper
{
class SlipperEngine final : public Application
{
 public:
    explicit SlipperEngine(ApplicationInfo &ApplicationInfo);

    void Run() override;
};
}  // namespace Slipper
