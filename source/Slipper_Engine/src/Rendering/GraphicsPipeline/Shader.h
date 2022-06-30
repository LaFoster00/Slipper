#pragma once

#include <memory>

#include "common_includes.h"

#include <string>
#include <unordered_map>
#include <vector>

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

// TODO pack all shader stages into this class for easier handling and make pipeline creation
// directly dependent on shader
class Shader
{
 public:
    Shader() = delete;
    Shader(Device &device,
           GraphicsEngine *graphicsPipeline,
           std::string_view filepath,
           ShaderType shaderType,
           size_t uniformBufferCount,
           VkDeviceSize BufferSize,
           VkDescriptorSetLayout descriptorSetLayout);
    void Destroy();

    std::vector<std::unique_ptr<UniformBuffer>> &CreateUniformBuffers(size_t count,
                                                                      VkDeviceSize BufferSize);
    void CreateDescriptorPool(size_t count);
    void CreateDescriptorSets(size_t count,
                              VkDescriptorSetLayout descriptorSetLayout,
                              VkDeviceSize bufferSize);

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

    static VkShaderModule CreateShaderModule(const std::vector<char> &code, Device &device);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(Shader &shader);

 public:
    Device &device;
    GraphicsEngine *graphicsPipeline;

    std::string name;
    ShaderType shaderType;
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo shaderStage;

    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
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
