#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/UniformBuffer.h"
#include "Presentation/SwapChain.h"
#include "RenderPass.h"

namespace Slipper
{
const char *ShaderTypeNames[]{"UNDEFINED", "Vertex", "Fragment", "Compute"};

Shader::Shader(const std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
               const std::optional<std::vector<RenderPass *>> RenderPasses)
{
    LoadShader(ShaderStagesCode);

    CreateDescriptorPool();
    CreateDescriptorSetLayouts();
    AllocateDescriptorSets();

    CreateUniformBuffers(Engine::MAX_FRAMES_IN_FLIGHT);

    if (RenderPasses.has_value()) {
        for (const auto render_pass : RenderPasses.value()) {
            RegisterRenderPass(render_pass);
        }
    }
}

Shader::~Shader()
{
    m_graphicsPipelines.clear();
    shaderLayout.reset();
    if (m_vkDescriptorPool)
        vkDestroyDescriptorPool(device, m_vkDescriptorPool, nullptr);

    for (const auto vk_descriptor_set_layout : m_vkDescriptorSetLayouts | std::views::values) {
        vkDestroyDescriptorSetLayout(device, vk_descriptor_set_layout, nullptr);
    }
    uniformBindingBuffers.clear();

    for (const auto &shader_stage : m_shaderStages | std::views::values) {
        vkDestroyShaderModule(device.logicalDevice, shader_stage.shaderModule, nullptr);
    }
}

GraphicsPipeline &Shader::RegisterRenderPass(NonOwningPtr<const RenderPass> RenderPass)
{
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve((m_vkDescriptorSetLayouts | std::ranges::views::values).size());
    for (auto descriptor_set_layout : m_vkDescriptorSetLayouts | std::ranges::views::values) {
        layouts.push_back(descriptor_set_layout);
    }

    return CreateGraphicsPipeline(RenderPass, layouts);
}

void Shader::Use(const VkCommandBuffer &CommandBuffer,
                 NonOwningPtr<const RenderPass> RenderPass,
                 VkExtent2D Extent) const
{
    if (!m_graphicsPipelines.contains(RenderPass)) {
        ASSERT(true, "RenderPass {} is not registered for this shader.", RenderPass->name)
    }

    auto &pipeline = m_graphicsPipelines.at(RenderPass);

    pipeline->Bind(CommandBuffer, Extent);

    const auto descriptor_sets = GetDescriptorSets();
    vkCmdBindDescriptorSets(CommandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->vkPipelineLayout,
                            0,
                            static_cast<uint32_t>(descriptor_sets.size()),
                            descriptor_sets.data(),
                            0,
                            nullptr);
}

template<>
bool Shader::BindShaderParameter(const std::string Name, const UniformBuffer &Object) const
{
    if (shaderLayout->namedLayoutBindings.contains(String::to_lower(Name))) {
        const auto binding = shaderLayout->namedLayoutBindings.at(String::to_lower(Name));

        ASSERT(binding->descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               "Descriptor '{}' is not of type buffer",
               Name);

        ASSERT(binding->size != Object.vkBufferSize,
               "Buffer '{}' size missmatch! Shader expects {} bytes but buffer has {} bytes.",
               Name,
               std::to_string(binding->size),
               std::to_string(Object.vkBufferSize));

        VkWriteDescriptorSet descriptor_write{};
        descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstBinding = binding->binding;
        descriptor_write.dstArrayElement = 0;
        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount = binding->descriptorCount;
        descriptor_write.pBufferInfo = Object.GetDescriptorInfo();

        // Update all descriptor sets at once
        std::vector<VkWriteDescriptorSet> descriptor_writes;
        descriptor_writes.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t frame = 0; frame < Engine::MAX_FRAMES_IN_FLIGHT; ++frame) {
            descriptor_write.dstSet = m_vkDescriptorSets.at(binding->set)[frame];
            descriptor_writes.push_back(descriptor_write);
        }
        vkUpdateDescriptorSets(device,
                               static_cast<uint32_t>(descriptor_writes.size()),
                               descriptor_writes.data(),
                               0,
                               nullptr);
        return true;
    }
    return false;
}

UniformBuffer *Shader::GetUniformBuffer(const std::string Name,
                                        const std::optional<uint32_t> Index) const
{
    const auto lowered_name = String::to_lower(Name);
    if (shaderLayout->namedLayoutBindings.contains(lowered_name)) {
        if (const auto binding = shaderLayout->namedLayoutBindings.at(lowered_name);
            uniformBindingBuffers.contains(GetHash(binding))) {

            return uniformBindingBuffers
                .at(GetHash(binding))[Index.has_value() ? Index.value() :
                                                          GraphicsEngine::Get().GetCurrentFrame()]
                .get();
        }
        ASSERT(true, "Uniform '{}' is not a buffer.", Name);
    }
    ASSERT(true, "Object '{}' does not exist.", Name);
}

std::vector<VkDescriptorSet> Shader::GetDescriptorSets(const std::optional<uint32_t> Index) const
{
    std::vector<VkDescriptorSet> ds;
    for (auto vk_descriptor_sets : m_vkDescriptorSets | std::ranges::views::values) {
        ds.push_back(
            vk_descriptor_sets[Index.has_value() ? Index.value() :
                                                   GraphicsEngine::Get().GetCurrentFrame()]);
    }
    return ds;
}

void Shader::LoadShader(const std::vector<std::tuple<std::string_view, ShaderType>> &Shaders)
{
    std::vector<std::vector<char>> shader_codes;
    shader_codes.reserve(Shaders.size());
    for (const auto &[filepath, shader_type] : Shaders) {
        ShaderStage new_shader_stage{};
        name = File::get_file_name_from_path(filepath);
        const auto &binary_code = shader_codes.emplace_back(File::read_binary_file(filepath));
        new_shader_stage.shaderModule = CreateShaderModule(binary_code);
        new_shader_stage.pipelineStageCrateInfo = CreateShaderStage(shader_type,
                                                                    new_shader_stage.shaderModule);
        m_shaderStages.insert(std::make_pair(shader_type, new_shader_stage));
        LOG_FORMAT("Create {} shader '{}' from {}",
                   ShaderTypeNames[static_cast<uint32_t>(shader_type)],
                   name,
                   filepath)
    }

    shaderLayout = std::make_unique<ShaderLayout>(shader_codes);
}

void Shader::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (const auto set_layout : shaderLayout->setLayouts) {
        for (DescriptorSetLayoutBinding binding : set_layout.bindings) {
            pool_sizes.push_back({binding.descriptorType, binding.descriptorCount});
        }
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = shaderLayout->setLayouts.size() * Engine::MAX_FRAMES_IN_FLIGHT;

    VK_ASSERT(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_vkDescriptorPool),
              "Failed to create descriptor pool!")
}

GraphicsPipeline &Shader::CreateGraphicsPipeline(
    NonOwningPtr<const RenderPass> RenderPass,
    const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts)
{
    std::vector<VkPipelineShaderStageCreateInfo> createInfos;
    createInfos.reserve(m_shaderStages.size());
    for (auto &shaderStage : m_shaderStages | std::views::values) {
        createInfos.push_back(shaderStage.pipelineStageCrateInfo);
    }

    return *m_graphicsPipelines
                .emplace(RenderPass,
                         new GraphicsPipeline(createInfos, RenderPass, DescriptorSetLayouts))
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
        case ShaderType::VERTEX:
            createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::FRAGMENT:
            createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::COMPUTE:
            createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
    }

    createInfo.module = ShaderModule;
    createInfo.pName = "main";

    return createInfo;
}

void Shader::CreateUniformBuffers(size_t Count)
{
    for (const auto layout_binding : shaderLayout->namedLayoutBindings | std::views::values) {
        if (layout_binding->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            auto &buffers = uniformBindingBuffers[GetHash(layout_binding)];
            for (int i = 0; i < Count; ++i) {
                buffers.emplace_back(std::make_unique<UniformBuffer>(layout_binding->size));
                BindShaderParameter(layout_binding->name, *buffers.back());
            }
        }
    }
}

void Shader::CreateDescriptorSetLayouts()
{
    for (auto set_layout : shaderLayout->setLayouts) {
        VkDescriptorSetLayout &new_layout = m_vkDescriptorSetLayouts[set_layout.setNumber];
        vkCreateDescriptorSetLayout(Device::Get(), &set_layout.createInfo, nullptr, &new_layout);
    }
}

void Shader::AllocateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve(m_vkDescriptorSetLayouts.size() * Engine::MAX_FRAMES_IN_FLIGHT);

    // Make a duplicate of each descriptor set layout for each frame so that they can be set on a
    // per frame basis
    for (auto vk_descriptor_set_layout : m_vkDescriptorSetLayouts | std::ranges::views::values) {
        std::fill_n(
            std::back_inserter(layouts), Engine::MAX_FRAMES_IN_FLIGHT, vk_descriptor_set_layout);
    }

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(layouts.size());

    VkDescriptorSetAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = m_vkDescriptorPool;
    allocate_info.pSetLayouts = layouts.data();
    allocate_info.descriptorSetCount = layouts.size();

    VK_ASSERT(vkAllocateDescriptorSets(Device::Get(), &allocate_info, descriptor_sets.data()),
              "Failed to allocate descriptor sets!");
    uint32_t i = 0;
    for (const auto &set_number : m_vkDescriptorSetLayouts | std::views::keys) {
        auto &set_layout = m_vkDescriptorSets[set_number];
        auto begin_offset_it = descriptor_sets.begin();
        std::advance(begin_offset_it, i * Engine::MAX_FRAMES_IN_FLIGHT);
        auto end_offset_it = descriptor_sets.begin();
        std::advance(end_offset_it,
                     i * Engine::MAX_FRAMES_IN_FLIGHT + Engine::MAX_FRAMES_IN_FLIGHT);
        set_layout.insert(set_layout.end(), begin_offset_it, end_offset_it);

        i++;
    }
}
}  // namespace Slipper