#pragma once

namespace Slipper::Editor
{
// Interface for the draw_editor functions
template<typename Editor, typename ComponentType> struct IComponentEditor
{
protected:
	virtual ~IComponentEditor() = default;

public:
	static Editor &Get()
    {
        static Editor editor;
        return editor;
    }

    static void Draw(entt::registry &Registry, const entt::entity Entity)
    {
        if (Registry.all_of<ComponentType>(Entity)) {
            auto &comp = Registry.get<ComponentType>(Entity);
            Get().DrawEditor(entt::type_id<ComponentType>(), comp);
        }
    }

    virtual void DrawEditor(entt::type_info Type, ComponentType &Component) = 0;
};
}  // namespace Slipper::Editor