#pragma once
#include "IComponentEditor.h"

namespace Slipper
{
struct Transform;
}

namespace Slipper::Editor
{
template<typename Editor>
concept ImplementsIComponentEditor =
    std::is_base_of_v<IComponentEditor<Editor, typename Editor::CType>, Editor>;

// Also works but std::is_base_of_v is much more easy to understand
/* requires(Editor e) {
    static_cast<const IComponentEditor<Editor, typename Editor::CType> *>(std::addressof(e));
};
*/

class EditorRegistry
{
    using EditorDrawFallback = std::function<void(const entt::type_info &)>;
    using EditorDrawFunc = std::function<void(entt::registry &, entt::entity)>;
    using EditorHeightMap = std::map<const entt::id_type, const uint32_t>;
    using EditorNameMap = std::map<const entt::id_type, std::string_view>;
    using EditorMap = std::unordered_map<const entt::type_info *, EditorDrawFunc>;
    using IdTypeToTypeInfoLookup = std::map<const entt::id_type, const entt::type_info *>;

 public:
    template<ImplementsIComponentEditor Editor> static void AddEditor()
    {
        const entt::type_info &type_id = Editor::GetComponentType();
        if (m_editors.contains(&type_id))
            return;

        m_editors.emplace(&type_id, Editor::Draw);
        const entt::id_type id = type_id.hash();
        m_idTypeTypeInfoLookup.emplace(id, &type_id);
        m_editorHeights.emplace(id, Editor::GetLines());
        m_editorNames.emplace(id, Editor::GetName());
    }

    // Return either actual editor or editor fallback
    static std::optional<EditorDrawFunc> TryGetEditor(entt::id_type Id);
    static std::optional<EditorDrawFunc> TryGetEditor(const entt::type_info &Type);
    static uint32_t GetEditorHeight(entt::id_type Id);
    static std::optional<uint32_t> TryGetEditorHeight(entt::id_type Id);
    static std::string_view GetEditorName(entt::id_type Id);
    static std::optional<std::string_view> TryGetEditorName(entt::id_type Id);

    static EditorDrawFallback &GetEditorFallback();

 private:
    static EditorMap m_editors;
    static EditorHeightMap m_editorHeights;
    static EditorNameMap m_editorNames;
    static IdTypeToTypeInfoLookup m_idTypeTypeInfoLookup;
    static EditorDrawFallback m_fallback;
};
}  // namespace Slipper::Editor
