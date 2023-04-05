#pragma once

#include "IComponentEditor.h"
#include "TransformComponent.h"

namespace Slipper::Editor
{
struct TransformEditor final : public IComponentEditor<TransformEditor, Transform>
{
    void DrawEditor(entt::type_info Type, Transform &Component) override;
    std::string_view Impl_GetName() override;
};
}  // namespace Slipper::Editor