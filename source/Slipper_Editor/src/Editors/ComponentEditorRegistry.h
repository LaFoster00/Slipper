#pragma once

namespace Slipper
{
struct Transform;
}

namespace Slipper::Editor
{
class EditorRegistry
{
    using EditorDrawFallback = std::function<void(const entt::type_info &)>;
    using EditorDrawFunc = std::function<void(entt::registry &, entt::entity)>;
    using EditorMap = std::unordered_map<const entt::type_info*, EditorDrawFunc>;
    using IdTypeToTypeInfoLookup = std::map<const entt::id_type, const entt::type_info*>;

 public:
    template<typename ComponentType> static void AddEditor(EditorDrawFunc EditorDrawFunc)
    {
        const entt::type_info &type_id = entt::type_id<ComponentType>();
        m_editors[&type_id] = EditorDrawFunc;
        const entt::id_type id = type_id.hash();
        m_idTypeTypeInfoLookup[id] = &type_id;
    }

    // Return either actual editor or editor fallback
    static std::optional<EditorDrawFunc> TryGetEditor(entt::id_type Id);
    static std::optional<EditorDrawFunc> TryGetEditor(const entt::type_info &Type);
    static EditorDrawFallback &GetEditorFallback();

 private:
    static EditorMap m_editors;
    static IdTypeToTypeInfoLookup m_idTypeTypeInfoLookup;
    static EditorDrawFallback m_fallback;
};
}  // namespace Slipper::Editor
