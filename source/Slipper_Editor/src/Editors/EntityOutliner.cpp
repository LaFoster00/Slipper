#include "EntityOutliner.h"

#include "ComponentEditorRegistry.h"
#include "TransformEditor.h"

namespace Slipper::Editor
{
void EntityOutliner::DrawEntity(Entity Entity)
{
    bool open = true;
    ImGui::Begin("Entity Outliner", &open);
    for (auto &&curr : EcsInterface::GetRegistry().storage()) {
        if (auto &storage = curr.second; storage.contains(Entity)) {
            const entt::id_type id = curr.first;
            if (auto editor = EditorRegistry::TryGetEditor(id)) {
                const auto &component_type = EcsInterface::GetComponentType(id);
                if (ImGui::CollapsingHeader(editor.value()->name.data())) {
                    ImGui::BeginGroup();
                    if (component_type.size != 0) {
                        editor.value()->drawFunc(EcsInterface::GetRegistry(), Entity);
                    }
                    // ImGui::EndChild();
                    ImGui::EndGroup();
                }
            }
            else {
                const auto &component_type = EcsInterface::GetComponentType(id);
                if (component_type.size == 1) {
                    EditorRegistry::GetEmptyEditor()(component_type.typeInfo);
                }
                else {
                    EditorRegistry::GetEditorFallback()(component_type.typeInfo);
                }
            }
        }
    }
    ImGui::End();
}
}  // namespace Slipper::Editor
