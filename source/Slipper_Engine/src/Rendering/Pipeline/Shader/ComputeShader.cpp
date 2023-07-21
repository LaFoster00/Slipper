#include "ComputeShader.h"

#include "ComputePipeline.h"

namespace Slipper
{
ComputeShader::ComputeShader(std::string_view ComputeShaderPath) : Shader()
{
    LoadShader(ComputeShaderPath);

    CreateDescriptorPool();
    CreateDescriptorSetLayouts();
    AllocateDescriptorSets();

    CreateUniformBuffers();
}

ComputeShader::~ComputeShader()
{
    m_computePipeline.reset();
    device.logicalDevice.destroyShaderModule(m_shaderStage.shaderModule, nullptr);
}

void ComputeShader::LoadShader(const std::string_view ShaderPath)
{
    ShaderStage new_shader_stage;
    name = File::get_file_name_from_path(ShaderPath);
    std::vector<char> shader_code = File::read_binary_file(ShaderPath);
    new_shader_stage.shaderModule = CreateShaderModule(shader_code);
    new_shader_stage.pipelineStageCrateInfo = CreateShaderStage(ShaderType::COMPUTE,
                                                                new_shader_stage.shaderModule);
    m_shaderStage = new_shader_stage;

    shaderLayout = new ShaderLayout({shader_code});
}

ComputePipeline &ComputeShader::CreateComputePipeline()
{
    m_computePipeline = new ComputePipeline((m_shaderStage).pipelineStageCrateInfo,
                                            m_vkDescriptorSetLayouts.begin()->second);
    return *m_computePipeline;
}
}  // namespace Slipper
