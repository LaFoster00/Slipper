#pragma once

namespace Slipper::Editor
{
// Interface for the draw_editor functions
template<typename ComponentType> struct IComponentEditor
{
    virtual ~IComponentEditor() = default;

    void Draw(entt::registry &Registry, const entt::entity Entity)
    {
        if (Registry.all_of<ComponentType>(Entity)) {
            auto &comp = Registry.get<ComponentType>(Entity);
            DrawEditor(entt::type_id<ComponentType>(), comp);
        }
    }

    virtual void DrawEditor(type_info Type, ComponentType &Component) = 0;
};
}  // namespace Slipper::Editor