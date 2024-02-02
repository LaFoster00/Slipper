#include "../vk_GraphicsShader.h"

#include "Vulkan/vk_GraphicsPipeline.h"
#include "Vulkan/vk_RenderPass.h"

namespace Slipper::GPU::Vulkan
{
    GraphicsShader::GraphicsShader(const std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStages,
                                   std::optional<std::vector<NonOwningPtr<RenderPass>>> RenderPasses)
    {
        LoadShader(ShaderStages);

        CreateDescriptorPool();
        CreateDescriptorSetLayouts();
        AllocateDescriptorSets();

        CreateUniformBuffers();

        if (RenderPasses.has_value())
        {
            for (const auto render_pass : RenderPasses.value())
            {
                RegisterRenderPass(render_pass);
            }
        }
    }

    GraphicsShader::~GraphicsShader()
    {
        m_graphicsPipelines.clear();

        for (const auto &shader_stage : m_shaderStages | std::views::values)
        {
            vkDestroyShaderModule(device.logicalDevice, shader_stage.shaderModule, nullptr);
        }
    }

    GraphicsPipeline &GraphicsShader::RegisterRenderPass(NonOwningPtr<const RenderPass> RenderPass)
    {
        std::vector<VkDescriptorSetLayout> layouts;
        layouts.reserve((m_vkDescriptorSetLayouts | std::ranges::views::values).size());
        for (auto descriptor_set_layout : m_vkDescriptorSetLayouts | std::ranges::views::values)
        {
            layouts.push_back(descriptor_set_layout);
        }

        return CreateGraphicsPipeline(RenderPass, layouts);
    }

    void GraphicsShader::Use(const vk::CommandBuffer &CommandBuffer,
                             NonOwningPtr<const RenderPass> RenderPass,
                             VkExtent2D Extent) const
    {
        if (!m_graphicsPipelines.contains(RenderPass))
        {
            ASSERT(true, "RenderPass {} is not registered for this shader.", RenderPass->name)
        }

        auto &pipeline = m_graphicsPipelines.at(RenderPass);

        pipeline->Bind(CommandBuffer, Extent);

        const auto descriptor_sets = GetDescriptorSets();
        CommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, pipeline->vkPipelineLayout, 0, GetDescriptorSets(), {});
    }

    void GraphicsShader::LoadShader(const std::vector<std::tuple<std::string_view, ShaderType>> &Shaders)
    {
        std::vector<std::vector<char>> shader_codes;
        shader_codes.reserve(Shaders.size());
        for (const auto &[filepath, shader_type] : Shaders)
        {
            ShaderStage new_shader_stage{};
            name = File::get_file_name_from_path(filepath);
            const auto &binary_code = shader_codes.emplace_back(File::read_binary_file(filepath));
            new_shader_stage.shaderModule = CreateShaderModule(binary_code);
            new_shader_stage.pipelineStageCrateInfo = CreateShaderStage(shader_type, new_shader_stage.shaderModule);
            m_shaderStages.insert(std::make_pair(shader_type, new_shader_stage));
            /* LOG_FORMAT("Create {} shader '{}' from {}",
                       ShaderTypeNames[static_cast<uint32_t>(shader_type)],
                       name,
                       filepath)*/
        }

        shaderLayout = new ShaderLayout(shader_codes);
    }

    GraphicsPipeline &GraphicsShader::CreateGraphicsPipeline(
        NonOwningPtr<const RenderPass> RenderPass, const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts)
    {
        std::vector<VkPipelineShaderStageCreateInfo> createInfos;
        createInfos.reserve(m_shaderStages.size());
        for (auto &shaderStage : m_shaderStages | std::views::values)
        {
            createInfos.push_back(shaderStage.pipelineStageCrateInfo);
        }

        return *m_graphicsPipelines
                    .emplace(RenderPass, new GraphicsPipeline(createInfos, RenderPass, DescriptorSetLayouts))
                    .first->second;
    }
}  // namespace Slipper::GPU::Vulkan
