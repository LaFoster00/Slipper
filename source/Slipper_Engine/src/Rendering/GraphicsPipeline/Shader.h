#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

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
    Shader(std::string_view name,
           std::vector<std::tuple<std::string_view, ShaderType>> &shaderStagesCode,
           size_t uniformBufferCount,
           VkDeviceSize BufferSize,
           VkDescriptorSetLayout descriptorSetLayout,
           std::optional<std::vector<RenderPass *>> RenderPasses = {},
           std::optional<VkExtent2D> RenderPassesExtent = {});
    ~Shader();

    GraphicsPipeline &RegisterForRenderPass(RenderPass *RenderPass, VkExtent2D extent);
    bool UnregisterFromRenderPass(RenderPass *renderPass);
    void ChangeResolutionForRenderPass(RenderPass *renderPass, VkExtent2D resolution);

    void Bind(const VkCommandBuffer &commandBuffer, RenderPass *RenderPass, uint32_t currentFrame) const;

    [[nodiscard]] UniformBuffer &GetUniformBuffer(size_t index) const
    {
        return *uniformBuffers[index];
    }

    [[nodiscard]] const VkDescriptorSet &GetDescriptorSet(size_t index) const
    {
        return descriptorSets[index];
    }

 private:
    void LoadShader(std::string_view filepath, ShaderType shaderType);

    GraphicsPipeline &CreateGraphicsPipeline(RenderPass *RenderPass,
                                             VkExtent2D &extent,
                                             VkDescriptorSetLayout descriptorSetLayout);

    static VkShaderModule CreateShaderModule(const std::vector<char> &code, Device &device);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(const ShaderType &shaderType,
                                                             const VkShaderModule &shaderModule);

    std::vector<std::unique_ptr<UniformBuffer>> &CreateUniformBuffers(size_t count,
                                                                      VkDeviceSize BufferSize);
    void CreateDescriptorPool(size_t count);
    void CreateDescriptorSets(size_t count,
                              VkDescriptorSetLayout descriptorSetLayout,
                              VkDeviceSize bufferSize);

 public:
    Device &device;

    std::string name;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

 private:
    std::unordered_map<ShaderType, ShaderStage> shaderStages;
    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
    std::unordered_map<RenderPass *, std::unique_ptr<GraphicsPipeline>> graphicsPipelines;
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
