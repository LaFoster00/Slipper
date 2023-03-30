#pragma once

namespace Slipper::Editor
{
class SceneOutliner
{

public:
    static void Draw();

	static Entity GetSelectedEntity()
    {
        return selected_entity;
    }

    static bool IsEntitySelected()
   {
       return selected_entity;
   }

private:
    static inline Entity selected_entity = Entity::InvalidEntity();
};
}  // namespace Slipper::Editor
