#pragma once

namespace Slipper
{
struct Transform;
}

namespace Slipper::Editor
{
class EditorRegistry
{
    using EditorDrawFallback = std::function<void(entt::type_info)>;
    using EditorDrawFunc = std::function<void(entt::registry &, entt::entity)>;
    using EditorMap = std::map<entt::type_info, EditorDrawFunc>;

 public:
    template<typename ComponentType> static void AddEditor(EditorDrawFunc EditorDrawFunc)
    {
        m_editors.insert(std::ref(entt::type_id<ComponentType>()), EditorDrawFunc);
    }

    // Return either actual editor or editor fallback 
    static std::optional<EditorDrawFunc> GetEditor(entt::type_info Type);
    static EditorDrawFallback &GetEditorFallback();

 private:
    static EditorMap m_editors;
    static EditorDrawFallback m_fallback;
};
}  // namespace Slipper::Editor
