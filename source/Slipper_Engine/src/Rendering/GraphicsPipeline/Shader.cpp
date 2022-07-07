#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/UniformBuffer.h"
#include "Setup/Device.h"
#include "Texture/Texture.h"
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
    std::array<VkDescriptorPoolSize, 2> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = static_cast<uint32_t>(Count);
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = static_cast<uint32_t>(Count);

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
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

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        // TODO Doing this like this is shit, move it into a proper parameter set function
        image_info.imageView = *GraphicsEngine::Get().textures[0];
        image_info.sampler = GraphicsEngine::Get().samplers[0];

        std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = descriptorSets[i];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &buffer_info;

        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = descriptorSets[i];
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pImageInfo = &image_info;

        vkUpdateDescriptorSets(device,
                               static_cast<uint32_t>(descriptor_writes.size()),
                               descriptor_writes.data(),
                               0,
                               nullptr);
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

        VkDescriptorSetLayoutBinding sampler_layout_binding{};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {mvp_layout_binding,
                                                                sampler_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings = bindings.data();

        VK_ASSERT(vkCreateDescriptorSetLayout(Device::Get(), &layout_info, nullptr, &layout),
                  "Failed to create descriptor set layout!")
    }

    return layout;
}
