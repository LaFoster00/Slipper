#include "EntityOutliner.h"

#include "ComponentEditorRegistry.h"

namespace Slipper::Editor
{
void EntityOutliner::DrawEntity(Entity &Entity)
{
    for (auto &ecs = EcsInterface::Get(); auto &&curr : ecs.GetRegistry().storage()) {
        if (auto &storage = curr.second; storage.contains(Entity)) {
            const entt::id_type id = curr.first;
            if (auto editor = EditorRegistry::TryGetEditor(id)) {
                const auto &type_info = Entity::GetComponentType(id);
                editor.value()(ecs.GetRegistry(), Entity);
            }
            else {
                const auto &type_info = Entity::GetComponentType(id);
                const auto &fallback_editor = EditorRegistry::GetEditorFallback();
                fallback_editor(type_info);
            }
        }
    }
}
}  // namespace Slipper::Editor
