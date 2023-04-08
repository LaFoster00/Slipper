#include "RendererUpdateSystem.h"

#include "RendererComponent.h"
#include "RenderingStage.h"
#include "TransformComponent.h"

namespace Slipper
{
void RendererUpdateSystem::Execute(entt::registry &Registry)
{
    Registry.view<Transform, Renderer>().each([](Transform &Transform, Renderer &Renderer) {
        for (auto render_pass : Renderer.stage->renderPasses) {
            Renderer.stage->SubmitDraw(
                render_pass, Renderer.material, Renderer.model, Transform.GetModelMatrix());
        }
    });
}
}  // namespace Slipper