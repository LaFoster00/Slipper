#include "Shader.h"

#include "File.h"
#include "GraphicsPipeline.h"
#include "Mesh/Mesh.h"
#include "Mesh/UniformBuffer.h"
#include "Setup/Device.h"
#include "ShaderReflection.h"
#include "Texture/Texture.h"
#include "common_defines.h"
#include "spirv_reflect.h"

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

    CreateUniformBuffers(Engine::MAX_FRAMES_IN_FLIGHT);

    if (RenderPasses.has_value()) {
        for (const auto render_pass : RenderPasses.value()) {
            RegisterForRenderPass(render_pass, RenderPassesExtent.value());
            CreateGraphicsPipeline(
                render_pass, RenderPassesExtent.value(), GetDescriptorSetLayouts());
        }
    }
}

Shader::~Shader()
{
    m_graphicsPipelines.clear();
    shaderModuleLayouts.clear();
    if (descriptorPool)
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
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

    return CreateGraphicsPipeline(RenderPass, Extent, GetDescriptorSetLayouts());
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
void Shader::ChangeResolutionForRenderPass(RenderPass *RenderPass, VkExtent2D Resolution)
{
    if (m_graphicsPipelines.contains(RenderPass)) {
        m_graphicsPipelines.at(RenderPass)->ChangeResolution(Resolution);
    }
}

void Shader::Bind(const VkCommandBuffer &CommandBuffer,
                  const RenderPass *RenderPass,
                  std::optional<uint32_t> CurrentFrame) const
{
    if (m_graphicsPipelines.contains(RenderPass)) {
        m_graphicsPipelines.at(RenderPass)->Bind(CommandBuffer);

        const auto descriptor_sets = GetDescriptorSets();
        vkCmdBindDescriptorSets(CommandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_graphicsPipelines.at(RenderPass)->vkPipelineLayout,
                                0,
                                descriptor_sets.size(),
                                descriptor_sets.data(),
                                0,
                                nullptr);
    }
    else {
        ASSERT(1, "renderPass is not registered for this shader.")
    }
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
                                               std::make_unique<ShaderLayout>(binary_code)));

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(ShaderType)] << " shader '"
              << name << "' from " << Filepath << '\n';
}

GraphicsPipeline &Shader::CreateGraphicsPipeline(
    RenderPass *RenderPass,
    VkExtent2D &Extent,
    const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts)
{
    std::vector<VkPipelineShaderStageCreateInfo> createInfos;
    createInfos.reserve(m_shaderStages.size());
    for (auto &[shaderType, shaderStage] : m_shaderStages) {
        createInfos.push_back(shaderStage.pipelineStageCrateInfo);
    }
    return *m_graphicsPipelines
                .emplace(
                    std::make_pair(RenderPass,
                                   std::make_unique<GraphicsPipeline>(
                                       createInfos, Extent, RenderPass, DescriptorSetLayouts)))
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
            if (layout_binding->binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            	auto &buffers = uniformBindingBuffers[layout_binding];
	            for (int i = 0; i < Count; ++i)
	            {
                    buffers.emplace_back(
                        std::make_unique<UniformBuffer>(layout_binding->size));
                    shader_module_layout->SetBindingUniformBuffer(layout_binding->name, *buffers.back());
	            }
            }
        }
    }
}

std::vector<VkDescriptorSetLayout> Shader::GetDescriptorSetLayouts() const
{
    std::vector<VkDescriptorSetLayout> layouts;
    for (auto &shader_layouts : shaderModuleLayouts | std::views::values) {
        layouts.insert(
            layouts.end(), shader_layouts->vkLayouts.begin(), shader_layouts->vkLayouts.end());
    }
    return layouts;
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
                  "Failed to create descriptor set vkLayout!")
    }

    return layout;
}

template<> void Shader::SetShaderUniform(const std::string Name, const UniformBuffer &Data)
{
	for (const auto &shader_module_layout : shaderModuleLayouts | std::views::values)
	{
        if (shader_module_layout->SetBindingUniformBuffer(Name, Data))
        {
            return;
        }
	}
    ASSERT(1, "No uniform buffer with name '", Name, "' found.");
}

template<> void Shader::SetShaderUniform(const std::string Name, const Texture &Data)
{
    for (const auto &shader_module_layout : shaderModuleLayouts | std::views::values) {
        if (shader_module_layout->SetBindingTexture(Name, Data)) {
            return;
        }
    }
    ASSERT(1, "No uniform texture with name '", Name, "' found.");
}