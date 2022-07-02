#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/UniformBuffer.h"
#include "Setup/Device.h"
#include "common_defines.h"

const char *ShaderTypeNames[]{"Vertex", "Fragment", "Compute"};

Shader::Shader(std::string_view Name,
               std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
               VkDeviceSize BufferSize,
               VkDescriptorSetLayout DescriptorSetLayout,
               std::optional<std::vector<RenderPass *>> RenderPasses,
               std::optional<VkExtent2D> RenderPassesExtent)
    : device(Device::Get()), name(Name), descriptorSetLayout(DescriptorSetLayout)
{
    for (auto &[filepath, shaderType] : ShaderStagesCode) {
        LoadShader(filepath, shaderType);
    }

    if (BufferSize > 0) {
        CreateUniformBuffers(Engine::MAX_FRAMES_IN_FLIGHT, BufferSize);
        CreateDescriptorPool(Engine::MAX_FRAMES_IN_FLIGHT);
        CreateDescriptorSets(Engine::MAX_FRAMES_IN_FLIGHT, DescriptorSetLayout, BufferSize);
    }

    if (RenderPasses.has_value()) {
        for (const auto renderPass : RenderPasses.value()) {
            CreateGraphicsPipeline(renderPass, RenderPassesExtent.value(), DescriptorSetLayout);
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
GraphicsPipeline &Shader::RegisterForRenderPass(RenderPass *RenderPass, VkExtent2D Extent)
{
    RenderPass->RegisterShader(this);
    return CreateGraphicsPipeline(RenderPass, Extent, descriptorSetLayout);
}

bool Shader::UnregisterFromRenderPass(RenderPass *RenderPass)
{
    if (graphicsPipelines.contains(RenderPass)) {
        RenderPass->UnregisterShader(this);
        graphicsPipelines.erase(RenderPass);
        return true;
    }
    return false;
}

// TODO change this to swap chain dependency instead of resolution, shader should be able to be
// bound to multiple render passes and swapchains
void Shader::ChangeResolutionForRenderPass(RenderPass *RenderPass, VkExtent2D Resolution)
{
    if (graphicsPipelines.contains(RenderPass)) {
        graphicsPipelines.at(RenderPass)->ChangeResolution(Resolution);
    }
}

void Shader::Bind(const VkCommandBuffer &CommandBuffer,
                  const RenderPass *RenderPass,
                  std::optional<uint32_t> CurrentFrame) const
{
    if (graphicsPipelines.contains(RenderPass)) {
        graphicsPipelines.at(RenderPass)->Bind(CommandBuffer);

        vkCmdBindDescriptorSets(CommandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipelines.at(RenderPass)->vkPipelineLayout,
                                0,
                                1,
                                &GetDescriptorSet(CurrentFrame),
                                0,
                                nullptr);
    }
    else {
        ASSERT(1, "renderPass is not registered for this shader.")
    }
}

void Shader::LoadShader(std::string_view Filepath, ShaderType ShaderType)
{
    ShaderStage newShaderStage{};
    name = File::get_file_name_from_path(Filepath);
    const auto binaryCode = File::read_binary_file(Filepath);
    newShaderStage.shaderModule = CreateShaderModule(binaryCode);
    newShaderStage.pipelineStageCrateInfo = CreateShaderStage(ShaderType,
                                                              newShaderStage.shaderModule);

    shaderStages.insert(std::make_pair(ShaderType, newShaderStage));

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(ShaderType)] << " shader '"
              << name << "' from " << Filepath << '\n';
}

GraphicsPipeline &Shader::CreateGraphicsPipeline(RenderPass *RenderPass,
                                                 VkExtent2D &Extent,
                                                 VkDescriptorSetLayout DescriptorSetLayout)
{
    std::vector<VkPipelineShaderStageCreateInfo> createInfos;
    createInfos.reserve(shaderStages.size());
    for (auto &[shaderType, shaderStage] : shaderStages) {
        createInfos.push_back(shaderStage.pipelineStageCrateInfo);
    }
    return *graphicsPipelines
                .emplace(std::make_pair(RenderPass,
                                        std::make_unique<GraphicsPipeline>(
                                            createInfos, Extent, RenderPass, DescriptorSetLayout)))
                .first->second;
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &Code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = Code.size();
    /* Reinterpret cast only works because std::vector allready uses worst case allignment for its
     * data. */
    create_info.pCode = reinterpret_cast<const uint32_t *>(Code.data());

    VkShaderModule shader_module;
    VK_ASSERT(vkCreateShaderModule(Device::Get(), &create_info, nullptr, &shader_module),
              "Failed to create shader module!");

    return shader_module;
}

VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(const ShaderType &ShaderType,
                                                          const VkShaderModule &ShaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    switch (ShaderType) {
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

    createInfo.module = ShaderModule;
    createInfo.pName = "main";

    return createInfo;
}

std::vector<std::unique_ptr<UniformBuffer>> &Shader::CreateUniformBuffers(size_t Count,
                                                                          VkDeviceSize BufferSize)
{
    for (int i = 0; i < Count; ++i) {
        uniformBuffers.emplace_back(std::make_unique<UniformBuffer>(BufferSize));
    }

    return uniformBuffers;
}

void Shader::CreateDescriptorPool(size_t Count)
{
    VkDescriptorPoolSize pool_size{};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = static_cast<uint32_t>(Count);

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = static_cast<uint32_t>(Count);

    VK_ASSERT(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool),
              "Failed to create descriptor pool!")
}

void Shader::CreateDescriptorSets(const size_t Count,
                                  const VkDescriptorSetLayout DescriptorSetLayout,
                                  const VkDeviceSize BufferSize)
{
    const std::vector<VkDescriptorSetLayout> layouts(Count, DescriptorSetLayout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptorPool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(Count);
    alloc_info.pSetLayouts = layouts.data();

    descriptorSets.resize(Count);
    VK_ASSERT(vkAllocateDescriptorSets(device, &alloc_info, descriptorSets.data()),
              "Failed to allocate descriptor sets!")

    for (int i = 0; i < Count; ++i) {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = *uniformBuffers[i];
        buffer_info.offset = 0;
        buffer_info.range = BufferSize;

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet = descriptorSets[i];
        descriptor_write.dstBinding = 0;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = 1;
        descriptor_write.pBufferInfo = &buffer_info;
        descriptor_write.pImageInfo = nullptr;        // Optional
        descriptor_write.pTexelBufferView = nullptr;  // Optional

        vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, nullptr);
    }
}

VkDescriptorSetLayout UniformMVP::GetDescriptorSetLayout()
{
    static VkDescriptorSetLayout layout = nullptr;
    if (!layout) {
        VkDescriptorSetLayoutBinding mvp_layout_binding{};
        mvp_layout_binding.binding = 0;
        mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvp_layout_binding.descriptorCount = 1;
        mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        mvp_layout_binding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &mvp_layout_binding;

        VK_ASSERT(vkCreateDescriptorSetLayout(Device::Get(), &layoutInfo, nullptr, &layout),
                  "Failed to create descriptor set layout!")
    }

    return layout;
}
