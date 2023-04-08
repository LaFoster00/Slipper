#include "Material.h"

#include "Shader/Shader.h"

namespace Slipper
{
Material::Material(NonOwningPtr<Shader> Shader)
{
    shader = Shader;
}

bool Material::SetUniform(std::string Name, IShaderBindableData &Uniform)
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
    }
    return false;
}

void Material::Use(const VkCommandBuffer &CommandBuffer,
                   NonOwningPtr<const RenderPass> RenderPass,
                   VkExtent2D Extent) const
{
    BindShaderUniforms();
    shader->Use(CommandBuffer, RenderPass, Extent);
}

UniformBuffer *Material::GetUniformBuffer(const std::string Name,
                                          const std::optional<uint32_t> Index) const
{
    return shader->GetUniformBuffer(Name, Index);
}

void Material::BindShaderUniforms() const
{
    for (auto &uniform : uniforms | std::views::values) {
        shader->BindShaderUniform(
            uniform.shaderBinding, *uniform.data, GraphicsEngine::Get().GetCurrentFrame());
    }
}
}  // namespace Slipper
