#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "GraphicsEngine.h"
#include "common_includes.h"

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

extern const char *ShaderTypeNames[];

class Device;
class GraphicsEngine;

class Shader
{
 public:
    struct ShaderStage
    {
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo pipelineStageCrateInfo;
    };

    Shader() = delete;
    Shader(std::string_view Name,
           std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
           VkDeviceSize BufferSize,
           VkDescriptorSetLayout DescriptorSetLayout,
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
            return *uniformBuffers[Index.value()];
        }
        return *uniformBuffers[GraphicsEngine::Get().currentFrame];
    }

    [[nodiscard]] const VkDescriptorSet &GetDescriptorSet(
        const std::optional<uint32_t> Index = {}) const
    {
        if (Index.has_value()) {
            return descriptorSets[Index.value()];
        }
        return descriptorSets[GraphicsEngine::Get().currentFrame];
    }

 private:
    void LoadShader(std::string_view Filepath, ShaderType ShaderType);

    GraphicsPipeline &CreateGraphicsPipeline(RenderPass *RenderPass,
                                             VkExtent2D &Extent,
                                             VkDescriptorSetLayout DescriptorSetLayout);

    static VkShaderModule CreateShaderModule(const std::vector<char> &Code);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &ShaderType,
                                                             const VkShaderModule &ShaderModule);

    std::vector<std::unique_ptr<UniformBuffer>> &CreateUniformBuffers(size_t Count,
                                                                      VkDeviceSize BufferSize);
    void CreateDescriptorPool(size_t Count);
    void CreateDescriptorSets(size_t Count,
                              VkDescriptorSetLayout DescriptorSetLayout,
                              VkDeviceSize BufferSize);

 public:
    Device &device;

    std::string name;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

 private:
    std::unordered_map<ShaderType, ShaderStage> shaderStages;
    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
    std::unordered_map<const RenderPass *, std::unique_ptr<GraphicsPipeline>> graphicsPipelines;
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
