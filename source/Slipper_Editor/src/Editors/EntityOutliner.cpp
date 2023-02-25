#include "EntityOutliner.h"

#include "ComponentEditorRegistry.h"

namespace Slipper::Editor
{
void EntityOutliner::DrawEntity(Entity &Entity)
{
	bool open = true;
    ImGui::Begin("Entity Outliner", &open);
    for (auto &ecs = EcsInterface::Get(); auto &&curr : ecs.GetRegistry().storage()) {
        if (auto &storage = curr.second; storage.contains(Entity)) {
            const entt::id_type id = curr.first;
            if (auto editor = EditorRegistry::TryGetEditor(id)) {
                const auto &type_info = Entity::GetComponentType(id);
                ImGui::BeginChild(type_info.name().data(),
                                  { ImGui::GetWindowContentRegionWidth(), 30.0f * EditorRegistry::GetEditorHeight(id)});
                ImGui::Text(EditorRegistry::GetEditorName(id).data());
                editor.value()(ecs.GetRegistry(), Entity);
                ImGui::EndChild();
            }
            else {
                const auto &type_info = Entity::GetComponentType(id);
                const auto &fallback_editor = EditorRegistry::GetEditorFallback();
                fallback_editor(type_info);
            }
        }
    }
    ImGui::End();
}
}  // namespace Slipper::Editor
