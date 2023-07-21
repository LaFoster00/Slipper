#pragma once
#include "Shader.h"

namespace Slipper
{
class ComputePipeline;
class RenderPass;
enum class ShaderType;

class ComputeShader : public Shader
{
    friend ShaderManager;
 public:
    ~ComputeShader() override;

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