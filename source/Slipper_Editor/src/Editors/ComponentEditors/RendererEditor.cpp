#include "RendererEditor.h"

#include "Material.h"
#include "Model/Model.h"
#include "Texture/Texture.h"
#include "TextureManager.h"

namespace Slipper::Editor
{
void RenderEditor::DrawEditor(entt::type_info Type, Renderer &Component)
{
    DrawModelEditor(*Component.model);
    DrawMaterialEditor(*Component.material);
}

void RenderEditor::DrawMaterialEditor(Material &Material)
{
    for (const auto &[binding, data] : Material.uniforms | std::views::values) {
        ImGui::Text(std::format("{}:\n\t{}\n ",
                                DescriptorTypeToString.at(binding.get().descriptorType),
                                binding.get().name.c_str())
                        .c_str());
        switch (binding.get().descriptorType) {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
                break;
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                DrawImageEditor(static_cast<Texture *>(data.get()), binding, Material);
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                break;
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                break;
            case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
                break;
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
                break;
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
                break;
            case VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM:
                break;
            case VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM:
                break;
            case VK_DESCRIPTOR_TYPE_MUTABLE_EXT:
                break;
            case VK_DESCRIPTOR_TYPE_MAX_ENUM:
                break;
            default:;
        }
    }
}

void RenderEditor::DrawImageEditor(const NonOwningPtr<Texture> Tex,
                                   const DescriptorSetLayoutBinding &Binding,
                                   Material &Material)
{
    auto &textures = TextureManager::GetTextures();

    static std::string current_texture = TextureManager::GetTextureName(Tex);
    const std::string selected_texture = current_texture;
    if (ImGui::BeginCombo("Texture", selected_texture.c_str())) {
        for (const auto &[texture_name, texture] : textures) {
            const bool is_selected = (current_texture == selected_texture);
            if (ImGui::Selectable(texture_name.c_str(), is_selected)) {
                current_texture = texture_name;
                Material.SetUniform(Binding.name, *texture);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static std::unordered_map<NonOwningPtr<Texture>, VkDescriptorSet> imgui_textures;
    if (!imgui_textures.contains(Tex)) {
        imgui_textures.emplace(
            Tex,
            ImGui_ImplVulkan_AddTexture(
                Tex->sampler, Tex->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }
    const uint32_t image_width = std::min(static_cast<int>(ImGui::GetWindowWidth()), 256);
    const float aspect = Tex->GetSize().y / Tex->GetSize().x;
    ImGui::Image(imgui_textures.at(Tex), ImVec2(image_width, image_width * aspect));

    ImGui::ShowDemoWindow();
}

void RenderEditor::DrawModelEditor(const Model &Model)
{
    ImGui::Text(Model.GetMesh().GetName().c_str());
    ImGui::Text(std::format("Num Vertices: {}", Model.GetMesh().NumVertex()).c_str());
    ImGui::Text(std::format("Num Indices: {}", Model.GetMesh().NumIndex()).c_str());
}
}  // namespace Slipper::Editor
