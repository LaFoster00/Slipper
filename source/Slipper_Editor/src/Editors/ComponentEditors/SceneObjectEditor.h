#pragma once
#include "IComponentEditor.h"
#include "SceneObjectComponent.h"

namespace Slipper::Editor
{
class SceneObjectEditor : public IComponentEditor<SceneObjectEditor, SceneObjectComponent>
{
 public:
    void DrawEditor(entt::type_info Type, SceneObjectComponent &Component) override;
    
    std::string_view Impl_GetName() override
    {
        return "Scene Object";
    }
};
}  // namespace Slipper::Editor