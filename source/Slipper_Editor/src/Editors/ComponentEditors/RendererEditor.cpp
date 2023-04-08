#include "RendererEditor.h"

#include "Material.h"
#include "Model/Model.h"

namespace Slipper::Editor
{
void RenderEditor::DrawEditor(entt::type_info Type, Renderer &Component)
{
    DrawModelEditor(*Component.model);
    DrawMaterialEditor(*Component.material);
}

void RenderEditor::DrawMaterialEditor(const Material &Material)
{
    auto shader_binding = [&](DescriptorSetLayoutBinding &Binding) {
        ImGui::Text(std::format("{}:\n\t{}\n ",
                                DescriptorTypeToString.at(Binding.descriptorType),
                                Binding.name.c_str())
                        .c_str());
    };
    for (const auto& [binding, data] : Material.uniforms | std::views::values) {
        shader_binding(binding);
    }
}

void RenderEditor::DrawModelEditor(const Model &Model)
{
    ImGui::Text(Model.GetMesh().GetName().c_str());
    ImGui::Text(std::format("Num Vertices: {}", Model.GetMesh().NumVertex()).c_str());
    ImGui::Text(std::format("Num Indices: {}", Model.GetMesh().NumIndex()).c_str());
}
}  // namespace Slipper::Editor
