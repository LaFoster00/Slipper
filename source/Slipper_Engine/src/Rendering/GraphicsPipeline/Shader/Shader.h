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

// DO NOT CHANGE NUMBERS WITHOUT CHANGING SHADERTYPENAMES ARRAY!!!
enum class ShaderType
{
    Vertex = 0,
    Fragment = 1,
    Compute = 2
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

extern const char *ShaderTypeNames[];

class Device;
class GraphicsEngine;

enum class ShaderMemberType : uint32_t
{
    UNDEFINED = 0x00000000,
    VOID = 0x00000001,
    BOOL = 0x00000002,
    INT = 0x00000004,
    FLOAT = 0x00000008,
    VECTOR = 0x00000100,
    MATRIX = 0x00000200,
    IMAGE = 0x00010000,
    SAMPLER = 0x00020000,
    SAMPLED_IMAGE = 0x00040000,
    BLOCK = 0x00080000,
    ACCELERATION_STRUCTURE = 0x00100000,
    EXTERNAL_MASK = 0x00FF0000,
    STRUCT = 0x10000000,
    ARRAY = 0x20000000,
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
    VkDescriptorType descriptorType;
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