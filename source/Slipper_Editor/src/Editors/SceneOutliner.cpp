#include "SceneOutliner.h"

#include "SceneObjectComponent.h"

namespace Slipper::Editor
{
void SceneOutliner::Draw()
{
    static bool open = true;
    ImGui::Begin("Scene Outliner", &open);
    for (Entity entity : EcsInterface::GetRegistry().view<SceneObjectComponent>()) {
        auto &scene_object = entity.GetComponent<SceneObjectComponent>();
        bool selected = selected_entity == entity;
        ImGui::Selectable(scene_object.GetName().c_str(), &selected);
        if (selected && selected_entity != entity) {
            selected_entity = entity;
        }
    }

    ImGui::End();
}
}  // namespace Slipper::Editor
