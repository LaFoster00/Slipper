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
           std::optional<std::vector<RenderPass *>> RenderPasses = {},
           std::optional<VkExtent2D> RenderPassesExtent = {});
    ~Shader();

    GraphicsPipeline &RegisterForRenderPass(RenderPass *RenderPass, VkExtent2D Extent);
    bool UnregisterFromRenderPass(RenderPass *RenderPass);
    void ChangeResolutionForRenderPass(RenderPass *RenderPass, VkExtent2D Resolution) const;

    /* Binds the shaders pipeline and its descriptor sets
     * Current frame is optional and will be fetched from the currentFrame of the GraphicsEngine if
     * empty.
     */
    void Use(const VkCommandBuffer &CommandBuffer, const RenderPass *RenderPass) const;

    bool BindShaderParameter(const std::string Name, const auto &Object) const
    {
        ASSERT(true, "You cant bind an object of type {}", typeid(decltype(Object)).name())
    }

    template<typename T>
        requires IsConvertibleToTexture<T> bool
    BindShaderParameter(const std::string Name, const T &Object) const;

    template<> bool BindShaderParameter(const std::string Name, const UniformBuffer &Object) const;
    template<typename T>
    bool BindShaderParameter(const std::string Name, const NonOwningPtr<T> &Object);
    template<typename T>
    bool BindShaderParameter(const std::string Name, const OwningPtr<T> &Object);
    template<typename T>
    bool BindShaderParameter(const std::string Name, const SharedPtr<T> &Object);

    [[nodiscard]] UniformBuffer *GetUniformBuffer(const std::string Name,
                                                  const std::optional<uint32_t> Index = {}) const;

    [[nodiscard]] std::vector<VkDescriptorSet> GetDescriptorSets(
        std::optional<uint32_t> Index = {}) const;

 private:
    void LoadShader(const std::vector<std::tuple<std::string_view, ShaderType>> &Shaders);

    void CreateDescriptorPool();
    void CreateDescriptorSetLayouts();
    void AllocateDescriptorSets();

    GraphicsPipeline &CreateGraphicsPipeline(
        RenderPass *RenderPass,
        VkExtent2D &Extent,
        const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);

    static VkShaderModule CreateShaderModule(const std::vector<char> &Code);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &ShaderType,
                                                             const VkShaderModule &ShaderModule);

    void CreateUniformBuffers(size_t Count);

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
    std::unordered_map<const RenderPass *, std::unique_ptr<GraphicsPipeline>> m_graphicsPipelines;
};

template<typename T>
    requires IsConvertibleToTexture<T>
inline bool Shader::BindShaderParameter(const std::string Name, const T &Object) const
{
    if (shaderLayout->namedLayoutBindings.contains(String::to_lower(Name))) {
        const auto binding = shaderLayout->namedLayoutBindings.at(String::to_lower(Name));

        ASSERT(binding->descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
               "Descriptor '{}' is not of type combined image sampler",
               Name);

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstBinding = binding->binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.descriptorCount = binding->descriptorCount;
        const auto image_info = Object.GetDescriptorImageInfo();
        descriptor_write.pImageInfo = &image_info;

        // Update all descriptor sets at once
        std::vector<VkWriteDescriptorSet> descriptor_writes;
        descriptor_writes.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t frame = 0; frame < Engine::MAX_FRAMES_IN_FLIGHT; ++frame) {
            descriptor_write.dstSet = m_vkDescriptorSets.at(binding->set)[frame];
            descriptor_writes.push_back(descriptor_write);
        }
        vkUpdateDescriptorSets(device,
                               static_cast<uint32_t>(descriptor_writes.size()),
                               descriptor_writes.data(),
                               0,
                               nullptr);
        return true;
    }
    return false;
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string Name, const NonOwningPtr<T> &Object)
{
    return BindShaderParameter(Name, *Object.get());
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string Name, const OwningPtr<T> &Object)
{
    return BindShaderParameter(Name, *Object.get());
}

template<typename T>
inline bool Shader::BindShaderParameter(const std::string Name, const SharedPtr<T> &Object)
{
    return BindShaderParameter(Name, *Object.get());
}

// extern template void Shader::BindShaderUniform(const std::string Name, const UniformBuffer
// &Data); extern template void Shader::BindShaderUniform(const std::string Name, const Texture
// &Data);
}  // namespace Slipper