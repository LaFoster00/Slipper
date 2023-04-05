#pragma once

#include "GraphicsEngine.h"
#include "ShaderLayout.h"

namespace Slipper
{
class ShaderLayout;
class ShaderReflection;
class RenderPass;
class GraphicsPipeline;
class UniformBuffer;

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

class Device;
class GraphicsEngine;

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

 public:
    struct ShaderStage
    {
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo pipelineStageCrateInfo;
    };

    Shader() = delete;
    Shader(const std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
           std::optional<std::vector<RenderPass *>> RenderPasses = {});
    ~Shader();

    GraphicsPipeline &RegisterRenderPass(NonOwningPtr<const RenderPass> RenderPass);

    /* Binds the shaders pipeline and its descriptor sets
     * Current frame is optional and will be fetched from the currentFrame of the GraphicsEngine if
     * empty.
     */
    void Use(const VkCommandBuffer &CommandBuffer,
             NonOwningPtr<const RenderPass> RenderPass,
             VkExtent2D Extent) const;

    bool BindShaderParameter(const std::string_view Name, const auto &Object, std::optional<uint32_t> Index = {}) const
    {
        ASSERT(true, "You cant bind an object of type {}", typeid(decltype(Object)).name())
    }

    template<typename T>
        requires IsConvertibleToTexture<T> bool
    BindShaderParameter(const std::string_view Name,
                        const T &Object,
                        std::optional<uint32_t> Index = {}) const;

    template<>
    bool BindShaderParameter(const std::string_view Name,
                             const UniformBuffer &Object,
                             std::optional<uint32_t> Index) const;
    template<typename T>
    bool BindShaderParameter(const std::string_view Name,
                             const NonOwningPtr<T> &Object,
                             std::optional<uint32_t> Index = {});
    template<typename T>
    bool BindShaderParameter(const std::string_view Name,
                             const OwningPtr<T> &Object,
                             std::optional<uint32_t> Index = {});
    template<typename T>
    bool BindShaderParameter(const std::string_view Name,
                             const SharedPtr<T> &Object,
                             std::optional<uint32_t> Index = {});

    [[nodiscard]] UniformBuffer *GetUniformBuffer(const std::string Name,
                                                  const std::optional<uint32_t> Index = {}) const;

    [[nodiscard]] std::vector<VkDescriptorSet> GetDescriptorSets(
        std::optional<uint32_t> Index = {}) const;

    [[nodiscard]] std::optional<Ref<DescriptorSetLayoutBinding>>
    GetNamedBinding(std::string_view Name) const
    {
        auto lowered_name = String::to_lower(Name);
        if (shaderLayout->namedLayoutBindings.contains(lowered_name)) {
            return {*shaderLayout->namedLayoutBindings.at(String::to_lower(Name))};
        }
        return {};
    }

 private:
    void LoadShader(const std::vector<std::tuple<std::string_view, ShaderType>> &Shaders);

    void CreateDescriptorPool();
    void CreateDescriptorSetLayouts();
    void AllocateDescriptorSets();

    void UpdateDescriptorSets(VkWriteDescriptorSet DescriptorWrite,
                              DescriptorSetLayoutBinding &Binding,
                              std::optional<uint32_t> Index) const;

    GraphicsPipeline &CreateGraphicsPipeline(
        NonOwningPtr<const RenderPass> RenderPass,
        const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);

    static VkShaderModule CreateShaderModule(const std::vector<char> &Code);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &ShaderType,
                                                             const VkShaderModule &ShaderModule);

    void CreateUniformBuffers();

 public:
    std::string name;
    std::unique_ptr<ShaderLayout> shaderLayout;
    std::map<DescriptorSetLayoutBinding::HashT, std::vector<std::unique_ptr<UniformBuffer>>>
        uniformBindingBuffers;

 private:
    VkDescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;
    std::map<uint32_t, std::vector<VkDescriptorSet>>
        m_vkDescriptorSets;  // One set for every frame
    std::unordered_map<ShaderType, ShaderStage> m_shaderStages;
    std::map<uint32_t, VkDescriptorSetLayout> m_vkDescriptorSetLayouts;  // One layout for set
    std::unordered_map<NonOwningPtr<const RenderPass>, OwningPtr<GraphicsPipeline>>
        m_graphicsPipelines;
};

template<typename T>
    requires IsConvertibleToTexture<T>
inline bool Shader::BindShaderParameter(const std::string_view Name,
                                        const T &Object,
                                        std::optional<uint32_t> Index) const
{
	if (auto hash_binding = GetNamedBinding(Name); hash_binding.has_value())
    {
        const Ref<DescriptorSetLayoutBinding> binding = hash_binding.value();

        ASSERT(binding.get().descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
               "Descriptor '{}' is not of type combined image sampler",
               Name);

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstBinding = binding.get().binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.descriptorCount = binding.get().descriptorCount;
        const auto image_info = Object.GetDescriptorImageInfo();
        descriptor_write.pImageInfo = &image_info;

        UpdateDescriptorSets(descriptor_write, binding, Index);
        return true;
    }
    return false;
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string_view Name,
                                        const NonOwningPtr<T> &Object,
                                        std::optional<uint32_t> Index)
{
    return BindShaderParameter(Name, *Object.get());
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string_view Name,
                                        const OwningPtr<T> &Object,
                                        std::optional<uint32_t> Index)
{
    return BindShaderParameter(Name, *Object.get());
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string_view Name,
                                        const SharedPtr<T> &Object,
                                        std::optional<uint32_t> Index)
{
    return BindShaderParameter(Name, *Object.get());
}

// extern template void Shader::BindShaderUniform(const std::string Name, const UniformBuffer
// &Data); extern template void Shader::BindShaderUniform(const std::string Name, const Texture
// &Data);
}  // namespace Slipper