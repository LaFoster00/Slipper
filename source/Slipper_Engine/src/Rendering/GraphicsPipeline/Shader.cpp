#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/UniformBuffer.h"
#include "Setup/Device.h"
#include "common_defines.h"

const char *ShaderTypeNames[]{"Vertex", "Fragment", "Compute"};

Shader::Shader(std::string_view name,
               std::vector<std::tuple<std::string_view, ShaderType>> &shaderStagesCode,
               VkDeviceSize BufferSize,
               VkDescriptorSetLayout descriptorSetLayout,
               std::optional<std::vector<RenderPass *>> RenderPasses,
               std::optional<VkExtent2D> RenderPassesExtent)
    : device(Device::Get()), name(name), descriptorSetLayout(descriptorSetLayout)
{
    for (auto &[filepath, shaderType] : shaderStagesCode) {
        LoadShader(filepath, shaderType);
    }

    if (BufferSize > 0) {
        CreateUniformBuffers(Engine::MaxFramesInFlight, BufferSize);
        CreateDescriptorPool(Engine::MaxFramesInFlight);
        CreateDescriptorSets(Engine::MaxFramesInFlight, descriptorSetLayout, BufferSize);
    }

    if (RenderPasses.has_value()) {
        for (const auto renderPass : RenderPasses.value()) {
            CreateGraphicsPipeline(renderPass, RenderPassesExtent.value(), descriptorSetLayout);
        }
    }
}

Shader::~Shader()
{
    graphicsPipelines.clear();

    if (descriptorPool)
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    uniformBuffers.clear();

    for (const auto &[shaderType, shaderStage] : shaderStages) {
        vkDestroyShaderModule(device.logicalDevice, shaderStage.shaderModule, nullptr);
    }
}

// TODO Introduce surface dependency since this is not only dependent on render pass
GraphicsPipeline &Shader::RegisterForRenderPass(RenderPass *RenderPass, VkExtent2D extent)
{
    RenderPass->RegisterShader(this);
    return CreateGraphicsPipeline(RenderPass, extent, descriptorSetLayout);
}

bool Shader::UnregisterFromRenderPass(RenderPass *renderPass)
{
    if (graphicsPipelines.contains(renderPass)) {
        renderPass->UnregisterShader(this);
        graphicsPipelines.erase(renderPass);
        return true;
    }
    return false;
}

// TODO change this to swap chain dependency instead of resolution, shader should be able to be
// bound to multiple render passes and swapchains
void Shader::ChangeResolutionForRenderPass(RenderPass *renderPass, VkExtent2D resolution)
{
    if (graphicsPipelines.contains(renderPass)) {
        graphicsPipelines.at(renderPass)->ChangeResolution(resolution);
    }
}

void Shader::Bind(const VkCommandBuffer &commandBuffer,
                  const RenderPass *renderPass,
                  std::optional<uint32_t> currentFrame) const
{
    if (graphicsPipelines.contains(renderPass)) {
        graphicsPipelines.at(renderPass)->Bind(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipelines.at(renderPass)->vkPipelineLayout,
                                0,
                                1,
                                &GetDescriptorSet(currentFrame),
                                0,
                                nullptr);
    }
    else {
        ASSERT(1, "renderPass is not registered for this shader.")
    }
}

void Shader::LoadShader(std::string_view filepath, ShaderType shaderType)
{
    ShaderStage newShaderStage{};
    name = File::GetFileNameFromPath(filepath);
    const auto binaryCode = File::ReadBinaryFile(filepath);
    newShaderStage.shaderModule = CreateShaderModule(binaryCode, device);
    newShaderStage.pipelineStageCrateInfo = CreateShaderStage(shaderType,
                                                              newShaderStage.shaderModule);

    shaderStages.insert(std::make_pair(shaderType, newShaderStage));

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(shaderType)] << " shader '"
              << name << "' from " << filepath << '\n';
}

GraphicsPipeline &Shader::CreateGraphicsPipeline(RenderPass *RenderPass,
                                                 VkExtent2D &extent,
                                                 VkDescriptorSetLayout descriptorSetLayout)
{
    std::vector<VkPipelineShaderStageCreateInfo> createInfos;
    createInfos.reserve(shaderStages.size());
    for (auto &[shaderType, shaderStage] : shaderStages) {
        createInfos.push_back(shaderStage.pipelineStageCrateInfo);
    }
    return *graphicsPipelines
                .emplace(std::make_pair(RenderPass,
                                        std::make_unique<GraphicsPipeline>(
                                            createInfos, extent, RenderPass, descriptorSetLayout)))
                .first->second;
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &code, Device &device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    /* Reinterpret cast only works because std::vector allready uses worst case allignment for its
     * data. */
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    VK_ASSERT(vkCreateShaderModule(device.logicalDevice, &createInfo, nullptr, &shaderModule),
              "Failed to create shader module!");

    return shaderModule;
}

VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(const ShaderType &shaderType,
                                                          const VkShaderModule &shaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    switch (shaderType) {
        case ShaderType::Vertex:
            createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::Fragment:
            createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::Compute:
            createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
    }

    createInfo.module = shaderModule;
    createInfo.pName = "main";

    return createInfo;
}

std::vector<std::unique_ptr<UniformBuffer>> &Shader::CreateUniformBuffers(size_t count,
                                                                          VkDeviceSize BufferSize)
{
    for (int i = 0; i < count; ++i) {
        uniformBuffers.emplace_back(std::make_unique<UniformBuffer>(BufferSize));
    }

    return uniformBuffers;
}

void Shader::CreateDescriptorPool(size_t count)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(count);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(count);

    VK_ASSERT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool),
              "Failed to create descriptor pool!")
}

void Shader::CreateDescriptorSets(size_t count,
                                  VkDescriptorSetLayout descriptorSetLayout,
                                  VkDeviceSize bufferSize)
{
    std::vector<VkDescriptorSetLayout> layouts(count, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(count);
    VK_ASSERT(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()),
              "Failed to allocate descriptor sets!")

    for (int i = 0; i < count; ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;        // Optional
        descriptorWrite.pTexelBufferView = nullptr;  // Optional

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}

VkDescriptorSetLayout UniformMVP::GetDescriptorSetLayout()
{
    static VkDescriptorSetLayout layout = nullptr;
    if (!layout) {
        VkDescriptorSetLayoutBinding mvpLayoutBinding{};
        mvpLayoutBinding.binding = 0;
        mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpLayoutBinding.descriptorCount = 1;
        mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        mvpLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &mvpLayoutBinding;

        VK_ASSERT(vkCreateDescriptorSetLayout(Device::Get(), &layoutInfo, nullptr, &layout),
                  "Failed to create descriptor set layout!")
    }

    return layout;
}
