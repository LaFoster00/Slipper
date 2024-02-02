#pragma once

#include "vk_Shader.h"

namespace Slipper::GPU::Vulkan
{
    class GraphicsPipeline;
    class RenderPass;
    enum class ShaderType;

    class GraphicsShader : public Shader
    {
        friend ShaderManager;

     public:
        ~GraphicsShader() override;

        /* Binds the shaders pipeline and its descriptor sets
         * Current frame is optional and will be fetched from the currentFrame of the GraphicsEngine if
         * empty.
         */
        void Use(const vk::CommandBuffer &CommandBuffer,
                 NonOwningPtr<const RenderPass> RenderPass,
                 VkExtent2D Extent) const;

        GraphicsPipeline &RegisterRenderPass(NonOwningPtr<const RenderPass> RenderPass);

     private:
        GraphicsShader() = delete;
        GraphicsShader(const std::vector<std::tuple<std::string_view, ShaderType>> &ShaderStages,
                       std::optional<std::vector<NonOwningPtr<RenderPass>>> RenderPasses = {});

        void LoadShader(const std::vector<std::tuple<std::string_view, ShaderType>> &Shaders);
        GraphicsPipeline &CreateGraphicsPipeline(NonOwningPtr<const RenderPass> RenderPass,
                                                 const std::vector<VkDescriptorSetLayout> &DescriptorSetLayouts);

     private:
        std::unordered_map<ShaderType, ShaderStage> m_shaderStages;
        std::unordered_map<NonOwningPtr<const RenderPass>, OwningPtr<GraphicsPipeline>> m_graphicsPipelines;
    };
}  // namespace Slipper::GPU::Vulkan
