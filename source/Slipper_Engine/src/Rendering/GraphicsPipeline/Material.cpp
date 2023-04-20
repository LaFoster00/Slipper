#include "Material.h"

#include "MaterialManager.h"
#include "Shader/Shader.h"

namespace Slipper
{
Material::Material(NonOwningPtr<Shader> Shader)
{
    shader = Shader;
}

bool Material::SetUniform(const std::string &Name, IShaderBindableData &Uniform)
{
    if (std::optional<Ref<DescriptorSetLayoutBinding>> binding = shader->GetNamedBinding(Name);
        binding.has_value()) {

        ASSERT(Uniform.GetDescriptorType() == binding.value().get().descriptorType,
               "Uniform that wants to bind to {} must be of type {} to bind to this material.",
               Name,
               static_cast<int>(binding.value().get().descriptorType))

        Uniform.AdditionalBindingChecks(binding.value());

        if (uniforms.contains(Name)) {
            auto &[shaderBinding, data] = uniforms.at(Name);
            data = &Uniform;
        }
        else {
            uniforms.emplace(Name, MaterialUniform{binding.value(), &Uniform});
        }

        MaterialManager::AddUniformUpdate(*this, uniforms.at(Name));
    }
    return false;
}

void Material::Use(const VkCommandBuffer &CommandBuffer,
                   NonOwningPtr<const RenderPass> RenderPass,
                   VkExtent2D Extent) const
{
    shader->Use(CommandBuffer, RenderPass, Extent);
}

UniformBuffer *Material::GetUniformBuffer(const std::string Name,
                                          const std::optional<uint32_t> Index) const
{
    return shader->GetUniformBuffer(Name, Index);
}

void Material::BindUniformForThisFrame(const MaterialUniform &Uniform) const
{
    shader->BindShaderUniform(
        Uniform.shaderBinding, *Uniform.data, GraphicsEngine::Get().GetCurrentFrame());
}
}  // namespace Slipper
