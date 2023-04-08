#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
class Material;
class RenderPass;
class RenderingStage;
class Model;

struct Renderer : public IEcsComponent<Renderer>
{
    Renderer(NonOwningPtr<RenderingStage> Stage,
             NonOwningPtr<Model> Model,
             NonOwningPtr<Material> Shader)
    {
        stage = Stage;
        model = Model;
        material = Shader;
    }

    NonOwningPtr<RenderingStage> stage;
    NonOwningPtr<Model> model;
    NonOwningPtr<Material> material;
};
}  // namespace Slipper