#pragma once
#include "IComponentEditor.h"
#include "RendererComponent.h"

namespace Slipper
{
class Texture;
struct DescriptorSetLayoutBinding;
}  // namespace Slipper

namespace Slipper::Editor
{
class RenderEditor : IComponentEditor<RenderEditor, Renderer>
{
 public:
    void DrawEditor(entt::type_info Type, Renderer &Component) override;

    std::string_view Impl_GetName() override
    {
        return "Renderer";
    }

 private:
    static void DrawModelEditor(const Model &Model);
    static void DrawMaterialEditor(Material &Material);
    static void DrawImageEditor(const NonOwningPtr<Texture> Tex,
                                const DescriptorSetLayoutBinding &Binding,
                                Material &Material);
};
}  // namespace Slipper::Editor
