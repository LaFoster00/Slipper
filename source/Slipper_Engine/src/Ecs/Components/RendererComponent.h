#pragma once
#include "IEcsComponent.h"

namespace Slipper
{
	class Shader;
	class Model;

struct Renderer : public IEcsComponent<Renderer>
{
    NonOwningPtr<Model> model;
    NonOwningPtr<Shader> shader;
};
}  // namespace Slipper