#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <ranges>
#include <unordered_map>

#include "Buffer.h"
#include "GraphicsEngine.h"
#include "ShaderLayout.h"
#include "common_defines.h"
#include "common_includes.h"

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

    [[nodiscard]] UniformBuffer *GetUniformBuffer(const std::string Name,
                                                  const std::optional<uint32_t> Index = {}) const
    {
        const auto lowered_name = String::to_lower(Name);
        for (auto &shader_module_layout : shaderModuleLayouts | std::views::values) {
            if (shader_module_layout->namedLayoutBindings.contains(lowered_name)) {
                if (const auto binding = shader_module_layout->namedLayoutBindings.at(
                        lowered_name);
                    uniformBindingBuffers.contains(binding)) {
                    if (Index.has_value()) {
                        return uniformBindingBuffers.at(binding)[Index.value()].get();
                    }
                    else {
                        return uniformBindingBuffers
                            .at(binding)[GraphicsEngine::Get().currentFrame]
                            .get();
                    }
                }
                ASSERT(true, "Uniform '", Name, "' is not a buffer.");
            }
        }
        ASSERT(true, "Object '", Name, "' does not exist.");
    }

    [[nodiscard]] std::vector<VkDescriptorSet> GetDescriptorSets(
        const std::optional<uint32_t> Index = {}) const
    {
        std::vector<VkDescriptorSet> ds;
        for (auto &shader_layout : shaderModuleLayouts | std::views::values) {
            for (auto descriptor_sets : shader_layout->descriptorSets | std::views::values) {
                if (Index.has_value()) {
                    ds.push_back(descriptor_sets[Index.value()]);
                }
                else {
                    ds.push_back(descriptor_sets[GraphicsEngine::Get().currentFrame]);
                }
            }
        }
        return ds;
    }

    template<typename DataType> void SetShaderUniform(const std::string Name, const DataType &Data)
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

    void CreateUniformBuffers(size_t Count);

    std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

 public:
    Device &device;

    std::string name;
    std::unordered_map<VkShaderModule, std::unique_ptr<ShaderLayout>> shaderModuleLayouts;
    std::unordered_map<DescriptorSetLayoutBinding *, std::vector<std::unique_ptr<UniformBuffer>>>
        uniformBindingBuffers;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

 private:
    std::unordered_map<ShaderType, ShaderStage> m_shaderStages;
    std::unordered_map<const RenderPass *, std::unique_ptr<GraphicsPipeline>> m_graphicsPipelines;
};

extern template void Shader::SetShaderUniform(const std::string Name, const UniformBuffer &Data);
extern template void Shader::SetShaderUniform(const std::string Name, const Texture &Data);