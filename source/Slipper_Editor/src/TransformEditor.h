#pragma once
#include "IComponentEditor.h"

namespace Slipper
{
	struct Transform;
}

namespace Slipper::Editor
{
struct TransformEditor final : public IComponentEditor<Transform>
{
    void DrawEditor(type_info Type, Transform &Component) override;
};
}  // namespace Slipper::Editor