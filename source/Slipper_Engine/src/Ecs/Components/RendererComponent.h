#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
    namespace GPU
    {
        class Material;
        class Model;
        class RenderingStage;
    }

    struct Renderer : public IEcsComponent<Renderer>
    {
        Renderer(NonOwningPtr<GPU::RenderingStage> Stage, NonOwningPtr<GPU::Model> Model, NonOwningPtr<GPU::Material> Shader)
        {
            stage = Stage;
            model = Model;
            material = Shader;
        }

        NonOwningPtr<GPU::RenderingStage> stage;
        NonOwningPtr<GPU::Model> model;
        NonOwningPtr<GPU::Material> material;
    };
}  // namespace Slipper
