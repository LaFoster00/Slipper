#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <ranges>
#include <unordered_map>

#include "Buffer.h"
#include "common_defines.h"
#include "GraphicsEngine.h"
#include "common_includes.h"

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

struct ShaderUniform
{
    virtual ~ShaderUniform() = default;
    virtual VkDescriptorSetLayout GetDescriptorSetLayout() = 0;

    // This is reuqired since the virtual function table takes up additional memory and only the
    // actual data is interesting to vulkan
    virtual size_t GetDataSize() const = 0;

    // This is required since the virtual function table takes up the front of the object.
    // Therefore the data access-point has to be specified manually
    virtual void const *GetData() const = 0;
};

struct UniformMVP : ShaderUniform
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    VkDescriptorSetLayout GetDescriptorSetLayout() override;

    size_t GetDataSize() const override
    {
        return sizeof(glm::mat4) * 3;
    }

    void const *GetData() const override
    {
        return &model;
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

struct ShaderMember
{
    std::string name;
    ShaderMemberType type;
    uint32_t size;
    uint32_t padded_size;
    std::vector<ShaderMember> members;
};

struct DescriptorSetLayoutBinding
{
    std::string name;
    std::vector<ShaderMember> members;
    uint32_t size;
    uint32_t padded_size;
    VkDescriptorSetLayoutBinding binding;
};

struct DescriptorSetLayoutInfo
{
    uint32_t setNumber;
    VkDescriptorSetLayout layout;
    VkDescriptorSetLayoutCreateInfo createInfo;
    std::vector<DescriptorSetLayoutBinding> bindings;

    std::vector<VkDescriptorSetLayoutBinding> GetVkBindings() const
    {
        std::vector<VkDescriptorSetLayoutBinding> vkBindings(bindings.size());
        for (int i = 0; i < vkBindings.size(); ++i)
        {
            vkBindings[i] = bindings[i].binding;
        }
        return vkBindings;
    }

    ~DescriptorSetLayoutInfo()
    {
        vkDestroyDescriptorSetLayout(Device::Get(), layout, nullptr);
    }
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
    void ChangeResolutionForRenderPass(RenderPass *RenderPass, VkExtent2D Resolution);

    /* Binds the shaders pipeline and its descriptor sets
     * Current frame is optional and will be fetched from the currentFrame of the GraphicsEngine if
     * empty.
     */
    void Bind(const VkCommandBuffer &CommandBuffer,
              const RenderPass *RenderPass,
              std::optional<uint32_t> CurrentFrame = {}) const;

    [[nodiscard]] UniformBuffer &GetUniformBuffer(const std::optional<uint32_t> Index = {}) const
    {
        if (Index.has_value()) {
            return *m_uniformBuffers[Index.value()];
        }
        return *m_uniformBuffers[GraphicsEngine::Get().currentFrame];
    }

    [[nodiscard]] const VkDescriptorSet &GetDescriptorSet(
        const std::optional<uint32_t> Index = {}) const
    {
        if (Index.has_value()) {
            return descriptorSets[Index.value()];
        }
        return descriptorSets[GraphicsEngine::Get().currentFrame];
    }

    template<typename DataType>
    void SetShaderUniform(const std::string_view Name, const DataType Data)
    {
        ASSERT(true, "DataType", typeid(DataType).name(), "is not suppored.")
    }

private:
    void LoadShader(std::string_view Filepath, ShaderType ShaderType);

    GraphicsPipeline &CreateGraphicsPipeline(
        RenderPass *RenderPass,
        VkExtent2D &Extent,
        const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);

    static VkShaderModule CreateShaderModule(const std::vector<char> &Code);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &ShaderType,
                                                             const VkShaderModule &ShaderModule);

    std::vector<std::unique_ptr<UniformBuffer>> &CreateUniformBuffers(size_t Count);
    void CreateDescriptorPool(size_t Count);
    void CreateDescriptorSets(size_t Count,
                              const std::vector<VkDescriptorSetLayout> &DescriptorSetLayout);

    const std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

 public:
    Device &device;

    std::string name;

    std::unordered_map<VkShaderModule, std::vector<DescriptorSetLayoutInfo *>>
        shaderDescriptorSetLayoutInfos;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

 private:
    std::unordered_map<ShaderType, ShaderStage> m_shaderStages;
    std::vector<std::unique_ptr<UniformBuffer>> m_uniformBuffers;
    std::unordered_map<const RenderPass *, std::unique_ptr<GraphicsPipeline>> m_graphicsPipelines;
};