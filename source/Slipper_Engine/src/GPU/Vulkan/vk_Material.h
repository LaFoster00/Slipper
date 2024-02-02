#pragma once

namespace Slipper
{
    class MaterialManager;
}

namespace Slipper::GPU::Vulkan
{
    class UniformBuffer;
    class RenderPass;
    class GraphicsShader;
    class IShaderBindableData;
    struct DescriptorSetLayoutBinding;

    struct MaterialUniform
    {
        Ref<DescriptorSetLayoutBinding> shaderBinding;
        NonOwningPtr<IShaderBindableData> data;
    };

    class Material
    {
        friend MaterialManager;

     public:
        Material(NonOwningPtr<GraphicsShader> Shader);

        bool SetUniform(const std::string &Name, IShaderBindableData &Uniform);

        void Use(const VkCommandBuffer &CommandBuffer,
                 NonOwningPtr<const RenderPass> RenderPass,
                 VkExtent2D Extent) const;

        [[nodiscard]] UniformBuffer *GetUniformBuffer(const std::string Name,
                                                      const std::optional<uint32_t> Index = {}) const;

     private:
        void BindUniformForThisFrame(const MaterialUniform &Uniform) const;

     public:
        NonOwningPtr<GraphicsShader> shader;
        // Uses string_view hash
        std::unordered_map<std::string, MaterialUniform> uniforms;
    };
}  // namespace Slipper::GPU::Vulkan
