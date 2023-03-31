#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
class RenderPass;
class RenderingStage;
class Shader;
class Model;

struct Renderer : public IEcsComponent<Renderer>
{
    Renderer(NonOwningPtr<RenderingStage> Stage,
             NonOwningPtr<Model> Model,
             NonOwningPtr<Shader> Shader)
    {
        stage = Stage;
        model = Model;
        shader = Shader;
    }
    
    NonOwningPtr<RenderingStage> stage;
    NonOwningPtr<Model> model;
    NonOwningPtr<Shader> shader;
};
}  // namespace Slipper