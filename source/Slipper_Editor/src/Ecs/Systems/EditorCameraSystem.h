#pragma once
#include "IEcsComponent.h"
#include "IEcsSystem.h"

namespace Slipper::Editor
{
struct EditorCameraComponent : public IEcsComponent<EditorCameraComponent>
{
};

struct EditorCameraSystem : public IEcsSystem<EditorCameraSystem>
{
	void Execute(entt::registry& Registry) override;
};
}  // namespace Slipper