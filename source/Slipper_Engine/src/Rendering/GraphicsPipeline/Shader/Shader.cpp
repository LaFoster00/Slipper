#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/UniformBuffer.h"
#include "Texture/Texture.h"
#include "spirv_reflect.h"

namespace Slipper
{
const char *ShaderTypeNames[]{"UNDEFINED", "Vertex", "Fragment", "Compute"};

Shader::Shader(std::string_view Name,
               std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStagesCode,
               const std::optional<std::vector<RenderPass *>> RenderPasses,
               std::optional<VkExtent2D> RenderPassesExtent)
    : device(Device::Get()), name(Name)
{
    for (auto &[filepath, shaderType] : ShaderStagesCode) {
        LoadShader(filepath, shaderType);
    }

    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    AllocateDescriptorSets();

    CreateUniformBuffers(Engine::MAX_FRAMES_IN_FLIGHT);

    if (RenderPasses.has_value()) {
        for (const auto render_pass : RenderPasses.value()) {
            RegisterForRenderPass(render_pass, RenderPassesExtent.value());
        }
    }
}

Shader::~Shader()
{
    m_graphicsPipelines.clear();
    shaderModuleLayouts.clear();
    if (m_vkDescriptorPool)
        vkDestroyDescriptorPool(device, m_vkDescriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(device, m_vkDescriptorSetLayout, nullptr);
    uniformBindingBuffers.clear();

    for (const auto &[shaderType, shaderStage] : m_shaderStages) {
        vkDestroyShaderModule(device.logicalDevice, shaderStage.shaderModule, nullptr);
    }
}

// TODO Introduce surface dependency since this is not only dependent on render pass
GraphicsPipeline &Shader::RegisterForRenderPass(RenderPass *RenderPass, VkExtent2D Extent)
{
    RenderPass->RegisterShader(this);
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts;

    return CreateGraphicsPipeline(RenderPass, Extent, m_vkDescriptorSetLayout);
}

bool Shader::UnregisterFromRenderPass(RenderPass *RenderPass)
{
    if (m_graphicsPipelines.contains(RenderPass)) {
        RenderPass->UnregisterShader(this);
        m_graphicsPipelines.erase(RenderPass);
        return true;
    }
    return false;
}

// TODO change this to swap chain dependency instead of resolution, shader should be able to be
// bound to multiple render passes and swapchains
void Shader::ChangeResolutionForRenderPass(RenderPass *RenderPass, VkExtent2D Resolution) const
{
    if (m_graphicsPipelines.contains(RenderPass)) {
        m_graphicsPipelines.at(RenderPass)->ChangeResolution(Resolution);
    }
}

void Shader::Use(const VkCommandBuffer &CommandBuffer, const RenderPass *RenderPass) const
{
    if (m_graphicsPipelines.contains(RenderPass)) {
        m_graphicsPipelines.at(RenderPass)->Bind(CommandBuffer);

        const auto descriptor_sets = GetDescriptorSets();
        vkCmdBindDescriptorSets(CommandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_graphicsPipelines.at(RenderPass)->vkPipelineLayout,
                                0,
                                static_cast<uint32_t>(descriptor_sets.size()),
                                descriptor_sets.data(),
                                0,
                                nullptr);
    }
    else {
        ASSERT(true, "RenderPass {} is not registered for this shader.", RenderPass->name)
    }
}

template<>
bool Shader::BindShaderParameter(const std::string Name, const UniformBuffer &Object) const
{
    for (auto &module_layout : shaderModuleLayouts | std::views::values) {
        if (module_layout->namedLayoutBindings.contains(String::to_lower(Name))) {
            const auto binding = module_layout->namedLayoutBindings.at(String::to_lower(Name));

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

            std::vector<VkWriteDescriptorSet> descriptor_writes;
            descriptor_writes.reserve(m_vkDescriptorSets.size() * Engine::MAX_FRAMES_IN_FLIGHT);
            for (const auto descriptor_set : m_vkDescriptorSets) {
                VkWriteDescriptorSet set_write = descriptor_write;
                set_write.dstSet = descriptor_set;
                descriptor_writes.push_back(set_write);
            }
            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptor_writes.size()),
                                   descriptor_writes.data(),
                                   0,
                                   nullptr);
            return true;
        }
    }
    return false;
}

template<> bool Shader::BindShaderParameter(const std::string Name, const Texture &Object) const
{
    for (auto &module_layout : shaderModuleLayouts | std::views::values) {
        if (module_layout->namedLayoutBindings.contains(String::to_lower(Name))) {
            const auto binding = module_layout->namedLayoutBindings.at(String::to_lower(Name));

            ASSERT(binding->descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                   "Descriptor '{}' is not of type combined image sampler",
                   Name);

            VkWriteDescriptorSet descriptor_write{};
            descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstBinding = binding->binding;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write.descriptorCount = binding->descriptorCount;
            const auto image_info = Object.GetDescriptorImageInfo();
            descriptor_write.pImageInfo = &image_info;

            std::vector<VkWriteDescriptorSet> descriptor_writes;
            descriptor_writes.reserve(m_vkDescriptorSets.size() * Engine::MAX_FRAMES_IN_FLIGHT);
            for (const auto descriptor_set : m_vkDescriptorSets) {
                VkWriteDescriptorSet set_write = descriptor_write;
                set_write.dstSet = descriptor_set;
                descriptor_writes.push_back(set_write);
            }
            vkUpdateDescriptorSets(device,
                                   static_cast<uint32_t>(descriptor_writes.size()),
                                   descriptor_writes.data(),
                                   0,
                                   nullptr);
            return true;
        }
    }
    return false;
}

UniformBuffer *Shader::GetUniformBuffer(const std::string Name,
                                        const std::optional<uint32_t> Index) const
{
    const auto lowered_name = String::to_lower(Name);
    for (auto &shader_module_layout : shaderModuleLayouts | std::views::values) {
        if (!shader_module_layout->namedLayoutBindings.contains(lowered_name)) {
            continue;
        }
        if (const auto binding = shader_module_layout->namedLayoutBindings.at(lowered_name);
            uniformBindingBuffers.contains(binding)) {

            return uniformBindingBuffers
                .at(binding)[Index.has_value() ? Index.value() :
                                                 GraphicsEngine::Get().currentFrame]
                .get();
        }
        ASSERT(true, "Uniform '{}' is not a buffer.", Name);
    }
    ASSERT(true, "Object '{}' does not exist.", Name);
    return nullptr;
}

std::vector<VkDescriptorSet> Shader::GetDescriptorSets(const std::optional<uint32_t> Index) const
{
    std::vector<VkDescriptorSet> ds;
    if (Index.has_value()) {
        ds.push_back(m_vkDescriptorSets[Index.value()]);
    }
    else {
        ds.push_back(m_vkDescriptorSets[GraphicsEngine::Get().currentFrame]);
    }
    return ds;
}

void Shader::LoadShader(std::string_view Filepath, ShaderType ShaderType)
{
    ShaderStage new_shader_stage{};
    name = File::get_file_name_from_path(Filepath);
    const auto binary_code = File::read_binary_file(Filepath);
    new_shader_stage.shaderModule = CreateShaderModule(binary_code);
    new_shader_stage.pipelineStageCrateInfo = CreateShaderStage(ShaderType,
                                                                new_shader_stage.shaderModule);

    m_shaderStages.insert(std::make_pair(ShaderType, new_shader_stage));
    shaderModuleLayouts.emplace(std::make_pair(new_shader_stage.shaderModule,
                                               std::make_unique<ShaderModuleLayout>(binary_code)));

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(ShaderType)] << " shader '"
              << name << "' from " << Filepath << '\n';
}

void Shader::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (const auto &module_layout : shaderModuleLayouts | std::views::values) {
        for (const auto set_layout : module_layout->setLayouts) {
            for (DescriptorSetLayoutBinding binding : set_layout.bindings) {
                pool_sizes.push_back(
                    VkDescriptorPoolSize{binding.descriptorType, binding.descriptorCount});
            }
        }
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = Engine::MAX_FRAMES_IN_FLIGHT * 2;

    VK_ASSERT(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_vkDescriptorPool),
              "Failed to create descriptor pool!")
}

GraphicsPipeline &Shader::CreateGraphicsPipeline(RenderPass *RenderPass,
                                                 VkExtent2D &Extent,
                                                 const VkDescriptorSetLayout DescriptorSetLayout)
{
    std::vector<VkPipelineShaderStageCreateInfo> createInfos;
    createInfos.reserve(m_shaderStages.size());
    for (auto &[shaderType, shaderStage] : m_shaderStages) {
        createInfos.push_back(shaderStage.pipelineStageCrateInfo);
    }
    return *m_graphicsPipelines
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
    for (const auto &shader_module_layout : shaderModuleLayouts | std::views::values) {
        for (const auto layout_binding :
             shader_module_layout->namedLayoutBindings | std::views::values) {
            if (layout_binding->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                auto &buffers = uniformBindingBuffers[layout_binding];
                for (int i = 0; i < Count; ++i) {
                    buffers.emplace_back(std::make_unique<UniformBuffer>(layout_binding->size));
                    BindShaderParameter(layout_binding->name, *buffers.back());
                }
            }
        }
    }
}

VkDescriptorSetLayout Shader::CreateDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto &shader_module_layout : shaderModuleLayouts | std::views::values) {
        for (auto set_layout : shader_module_layout->setLayouts) {
            bindings.insert(
                bindings.end(), set_layout.vkBindings.begin(), set_layout.vkBindings.end());
        }
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    createInfo.pBindings = bindings.data();
    vkCreateDescriptorSetLayout(Device::Get(), &createInfo, nullptr, &m_vkDescriptorSetLayout);

    return m_vkDescriptorSetLayout;
}

void Shader::AllocateDescriptorSets()
{
    const std::vector layouts(Engine::MAX_FRAMES_IN_FLIGHT, m_vkDescriptorSetLayout);

    VkDescriptorSetAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = m_vkDescriptorPool;
    allocate_info.pSetLayouts = layouts.data();
    allocate_info.descriptorSetCount = Engine::MAX_FRAMES_IN_FLIGHT;

    m_vkDescriptorSets.resize(layouts.size());
    VK_ASSERT(vkAllocateDescriptorSets(Device::Get(), &allocate_info, m_vkDescriptorSets.data()),
              "Failed to allocate descriptor sets!");
}
}  // namespace Slipper