#pragma once

namespace Slipper::Editor
{

using EditorDrawFallback = std::function<void(const entt::type_info &)>;
using EditorDrawFunc = std::function<void(entt::registry &, entt::entity)>;

inline EditorDrawFallback FallbackDraw = [](const entt::type_info &Type) {
    bool open = true;
    ImGui::Begin("Fallback Editor", &open);
    ImGui::Text(
        "There is no editor registered for component of type: %s \n Consider adding one and "
        "registering it using the EditorRegistry::AddEditor() function!",
        Type.name().data());
    ImGui::End();
};

struct EditorInfo
{
    EditorInfo(){};

    EditorInfo(const EditorDrawFunc DrawFunc,
               uint32_t Height,
               const std::string_view Name,
               const entt::type_info *ComponentType)
        : drawFunc(DrawFunc), height(Height), name(Name)
    {
        Map()[ComponentType] = this;
        const entt::id_type id = ComponentType->hash();
        IdToInfoLookup()[id] = ComponentType;

        std::cout << "Editor Registered: " << name << '\n';
    }

    static std::unordered_map<const entt::type_info *, const EditorInfo *> &Map()
    {
        static std::unordered_map<const entt::type_info *, const EditorInfo *> map;
        return map;
    }

    static std::map<const entt::id_type, const entt::type_info *> &IdToInfoLookup()
    {
        static std::map<const entt::id_type, const entt::type_info *> id_to_info_lookup;
        return id_to_info_lookup;
    }

    const EditorDrawFunc drawFunc;
    const uint32_t height = 0;
    const std::string_view name;
};

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

 public:
    static inline const EditorInfo Info = EditorInfo(&Draw, GetLines(), GetName(), &GetComponentType());
};

template<typename Editor>
concept ImplementsIComponentEditor =
    std::is_base_of_v<IComponentEditor<Editor, typename Editor::CType>, Editor>;

// Also works but std::is_base_of_v is much more easy to understand
/* requires(Editor e) {
    static_cast<const IComponentEditor<Editor, typename Editor::CType> *>(std::addressof(e));
};
*/
}  // namespace Slipper::Editor