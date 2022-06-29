#include "Shader.h"

#include "../GraphicsEngine.h"
#include "../Setup/Device.h"
#include "Filesystem/File.h"
#include "common_defines.h"
#include <cstring>

const char *ShaderTypeNames[]{"Vertex", "Fragment", "Compute"};

Shader::Shader(Device &device,
               GraphicsEngine *graphicsPipeline,
               std::string_view filepath,
               ShaderType shaderType)
    : device(device)
{
    this->graphicsPipeline = graphicsPipeline;

    LoadShader(filepath, shaderType);
}

void Shader::Destroy()
{
    size_t index;
    std::vector<VkPipelineShaderStageCreateInfo> &shaderstages = graphicsPipeline->vkShaderStages;
    for (size_t i = 0; i < shaderstages.size(); i++) {
        if (shaderstages[i].module == shaderModule) {
            shaderstages.erase(std::next(shaderstages.begin(), i));
            break;
        }
    }

    vkDestroyShaderModule(device.logicalDevice, shaderModule, nullptr);
}

void Shader::LoadShader(std::string_view filepath, ShaderType shaderType)
{
    name = File::GetFileNameFromPath(filepath);
    this->shaderType = shaderType;
    const auto binaryCode = File::ReadBinaryFile(filepath);
    shaderModule = CreateShaderModule(binaryCode, device);
    shaderStage = CreateShaderStage(*this);
    graphicsPipeline->vkShaderStages.push_back(shaderStage);

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(shaderType)] << " shader '"
              << name << "' from " << filepath << '\n';
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &code, Device &device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    /* Reinterpret cast only works because std::vector allready uses worst case allignment for its
     * data. */
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    VK_ASSERT(vkCreateShaderModule(device.logicalDevice, &createInfo, nullptr, &shaderModule),
              "Failed to create shader module!");

    return shaderModule;
}

VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(Shader &shader)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    switch (shader.shaderType) {
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

    createInfo.module = shader.shaderModule;
    createInfo.pName = "main";

    return createInfo;
}