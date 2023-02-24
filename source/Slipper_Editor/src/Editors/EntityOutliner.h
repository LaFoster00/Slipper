#pragma once

namespace Slipper
{
struct Entity;
}

namespace Slipper::Editor
{
class EntityOutliner
{
 public:
    static void DrawEntity(Entity &Entity);
};
}  // namespace Slipper::Editor