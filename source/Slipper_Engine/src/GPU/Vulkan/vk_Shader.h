#pragma once

#include "vk_IShaderBindableData.h"
#include "vk_ShaderLayout.h"
#include "vk_UniformBuffer.h"

namespace Slipper
{
    class ShaderManager;
    class ShaderReflection;
}

namespace Slipper::GPU::Vulkan
{
    extern const char *ShaderTypeNames[];

    // DO NOT CHANGE NUMBERS WITHOUT CHANGING SHADERTYPENAMES ARRAY!!!
    enum class ShaderType
    {
        UNDEFINED = 0,
        VERTEX = 1,
        FRAGMENT = 2,
        COMPUTE = 3
    };

    struct ShaderUniformObject
    {
     protected:
        ShaderUniformObject() = default;

     public:
        // This is reuqired since the virtual function table takes up additional memory and only the
        // actual data is interesting to vulkan
        virtual size_t GetDataSize() const = 0;

        // This is required since the virtual function table takes up the front of the object.
        // Therefore the data access-point has to be specified manually
        virtual void const *GetData() const = 0;
    };

    struct UniformVP final : ShaderUniformObject
    {
        glm::mat4 view = {};
        glm::mat4 projection = {};

        size_t GetDataSize() const override
        {
            return sizeof(UniformVP);
        }

        void const *GetData() const override
        {
            return &view;
        }
    };

    enum class ShaderMemberType : uint32_t
    {
        UNDEFINED,
        VOID,
        BOOL,
        INT,
        UINT,
        FLOAT,
        DOUBLE,
        VEC2,
        VEC3,
        VEC4,
        DVEC2,
        DVEC3,
        DVEC4,
        MATRIX2,
        MATRIX3,
        MATRIX4,
        IMAGE,
        SAMPLER,
        SAMPLED_IMAGE,
        ACCELERATION_STRUCTURE,
        STRUCT,
        ARRAY,
    };

    template<typename T>
    concept IsConvertibleToTexture = std::is_convertible_v<T *, Texture *>;

    class Shader : public DeviceDependentObject
    {
        friend ShaderReflection;
        friend ShaderManager;

     public:
        struct ShaderStage
        {
            VkShaderModule shaderModule;
            VkPipelineShaderStageCreateInfo pipelineStageCrateInfo;
        };

        virtual ~Shader();

        std::optional<Ref<DescriptorSetLayoutBinding>> BindShaderUniform(const std::string_view Name,
                                                                         const IShaderBindableData &Object,
                                                                         std::optional<uint32_t> Index = {}) const
        {
            if (const auto optional_binding = GetNamedBinding(Name); optional_binding.has_value())
            {
                const Ref<DescriptorSetLayoutBinding> binding = optional_binding.value();

                BindShaderUniform(binding, Object, Index);
                return optional_binding;
            }
            return {};
        }

        void BindShaderUniform(const DescriptorSetLayoutBinding &Binding,
                               const IShaderBindableData &Object,
                               std::optional<uint32_t> Index = {}) const
        {
            ASSERT(Binding.descriptorType == Object.GetDescriptorType(),
                   "Descriptor '{}' is not of type {}",
                   Binding.name,
                   vk::to_string(Object.GetDescriptorType()));

            Object.AdditionalBindingChecks(Binding);
            BindShaderUniform_Interface(Binding, Object, Index);
        }

        void BindShaderUniform(const DescriptorSetLayoutBindingMinimal &Binding,
                               const IShaderBindableData &Object,
                               std::optional<uint32_t> Index = {}) const
        {
            ASSERT(Binding.descriptorType == Object.GetDescriptorType(),
                   "Descriptor '{}' is not of type {}",
                   Binding.binding,
                   vk::to_string(Object.GetDescriptorType()));

            Object.AdditionalBindingChecks(Binding);
            BindShaderUniform_Interface(Binding, Object, Index);
        }

        [[nodiscard]] UniformBuffer *GetUniformBuffer(const std::string Name,
                                                      const std::optional<uint32_t> Index = {}) const;

        [[nodiscard]] std::vector<vk::DescriptorSet> GetDescriptorSets(std::optional<uint32_t> Index = {}) const;

        [[nodiscard]] std::optional<Ref<DescriptorSetLayoutBinding>> GetNamedBinding(std::string_view Name) const
        {
            auto lowered_name = String::to_lower(Name);
            if (shaderLayout->namedLayoutBindings.contains(lowered_name))
            {
                return {*shaderLayout->namedLayoutBindings.at(String::to_lower(Name))};
            }
            return {};
        }

     protected:
        void CreateDescriptorPool();
        void CreateDescriptorSetLayouts();
        void AllocateDescriptorSets();

        void BindShaderUniform_Interface(const DescriptorSetLayoutBindingMinimal &Binding,
                                         const IShaderBindableData &Object,
                                         std::optional<uint32_t> Index = {}) const;
        void UpdateDescriptorSets(vk::WriteDescriptorSet DescriptorWrite,
                                  const DescriptorSetLayoutBindingMinimal &Binding,
                                  std::optional<uint32_t> Index) const;

        static VkShaderModule CreateShaderModule(const std::vector<char> &Code);
        static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &ShaderType,
                                                                 const VkShaderModule &ShaderModule);

        void CreateUniformBuffers();

     public:
        std::string name;
        OwningPtr<ShaderLayout> shaderLayout;
        std::map<DescriptorSetLayoutBinding::HashT, std::vector<OwningPtr<UniformBuffer>>> uniformBindingBuffers;

     protected:
        vk::DescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;
        std::map<uint32_t, std::vector<vk::DescriptorSet>> m_vkDescriptorSets;  // One set for every frame

        std::map<uint32_t, vk::DescriptorSetLayout> m_vkDescriptorSetLayouts;  // One layout for set
    };

    // extern template void Shader::BindShaderUniform(const std::string Name, const UniformBuffer
    // &Data); extern template void Shader::BindShaderUniform(const std::string Name, const Texture
    // &Data);
}  // namespace Slipper::GPU::Vulkan
