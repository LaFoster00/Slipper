#pragma once
#include "IEcsSystem.h"

namespace Slipper
{
struct RendererUpdateSystem : public IEcsSystem<RendererUpdateSystem>
{
    void Execute(entt::registry &Registry) override;
};
}  // namespace Slipper