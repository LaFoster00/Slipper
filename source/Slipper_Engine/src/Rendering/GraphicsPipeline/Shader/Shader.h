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

class Shader
{
    friend ShaderReflection;

 public:
    struct ShaderStage
    {
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo pipelineStageCrateInfo;
    };

    Shader() = delete;
    Shader(std::string_view Name,
           std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
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

    template<> bool BindShaderParameter(const std::string Name, const UniformBuffer &Object) const;
    template<> bool BindShaderParameter(const std::string Name, const Texture &Object) const;

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
    Device &device;

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

// extern template void Shader::BindShaderUniform(const std::string Name, const UniformBuffer
// &Data); extern template void Shader::BindShaderUniform(const std::string Name, const Texture
// &Data);
}  // namespace Slipper