#pragma once

#include "vk_Shader.h"

namespace Slipper::GPU::Vulkan
{
class ComputePipeline;
class RenderPass;
enum class ShaderType;

class ComputeShader : public Shader
{
    friend ShaderManager;

 public:
    ~ComputeShader() override;

    void Dispatch(vk::CommandBuffer CommandBuffer,
                  uint32_t GroupCountX,
                  uint32_t GroupCountY,
                  uint32_t GroupCountZ) const;

 private:
    ComputeShader() = delete;
    ComputeShader(std::string_view ComputeShaderPath);

    void LoadShader(const std::string_view ShaderPath);
    ComputePipeline &CreateComputePipeline();

 private:
    ShaderStage m_shaderStage;
    OwningPtr<ComputePipeline> m_computePipeline;
};
}  // namespace Slipper