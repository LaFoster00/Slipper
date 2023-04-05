#include "RendererEditor.h"

#include "Model/Model.h"
#include "Shader/Shader.h"

namespace Slipper::Editor
{
void RenderEditor::DrawEditor(entt::type_info Type, Renderer &Component)
{
    DrawModelEditor(*Component.model);
    DrawShaderEditor(*Component.shader);
}

void RenderEditor::DrawShaderEditor(const Shader &Shader)
{
    auto shader_binding = [&](DescriptorSetLayoutBinding &Binding) {
        ImGui::Text(std::format("{}:\n\t{}\n ",
                                DescriptorTypeToString.at(Binding.descriptorType),
                                Binding.name.c_str())
                        .c_str());
    };
    for (auto &binding : Shader.shaderLayout->namedLayoutBindings) {
        shader_binding(*binding.second);
    }
}

void RenderEditor::DrawModelEditor(const Model &Model)
{
    ImGui::Text(Model.GetMesh().GetName().c_str());
    ImGui::Text(std::format("Num Vertices: {}", Model.GetMesh().NumVertex()).c_str());
    ImGui::Text(std::format("Num Indices: {}", Model.GetMesh().NumIndex()).c_str());
}
}  // namespace Slipper::Editor
