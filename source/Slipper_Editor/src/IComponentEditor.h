#pragma once

namespace Slipper::Editor
{
// Interface for the draw_editor functions
template<typename Editor, typename ComponentType> struct IComponentEditor
{
protected:
	virtual ~IComponentEditor() = default;

public:
    // The ComponentType for use in template functions
    using CType = ComponentType;
    using ImplementationInterfaceType = IComponentEditor<Editor, ComponentType>;

	static Editor &Get()
    {
        static Editor editor;
        return editor;
    }

    static const entt::type_info &GetComponentType()
	{
        static const entt::type_info type = entt::type_id<ComponentType>();
        return type;
	}

    static void Draw(entt::registry &Registry, const entt::entity Entity)
    {
        if (Registry.all_of<ComponentType>(Entity)) {
            auto &comp = Registry.get<ComponentType>(Entity);
            Get().DrawEditor(entt::type_id<ComponentType>(), comp);
        }
    }

    static uint32_t GetLines()
	{
        return Get().Impl_GetLines();
	}

    static std::string_view GetName()
	{
        return Get().Impl_GetName();
	}

    virtual void DrawEditor(entt::type_info Type, ComponentType &Component) = 0;
    virtual uint32_t Impl_GetLines() = 0;
    virtual std::string_view Impl_GetName() = 0;
};
}  // namespace Slipper::Editor