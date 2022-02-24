#include "Shader.h"

#include "Device.h"
#include "Filesystem/File.h"
#include <cstring>

const char *ShaderTypeNames[]{
    "Vertex",
    "Fragment",
    "Compute"};

std::unordered_map<const Device *, std::vector<VkPipelineShaderStageCreateInfo>> Shader::ShaderStages;

Shader::Shader()
{
    shaderModule = VK_NULL_HANDLE;
}

Shader::Shader(const char *filepath, ShaderType shaderType, Device *device)
{
    owningDevice = device;
    LoadShader(filepath, shaderType, device);
}

void Shader::Destroy()
{
    size_t index;
    std::vector<VkPipelineShaderStageCreateInfo> &shaderstages = ShaderStages[owningDevice];
    for (size_t i = 0; i < shaderstages.size(); i++)
    {
        if (shaderstages[i].module == shaderModule)
        {
            shaderstages.erase(std::next(shaderstages.begin(), i));
            break;
        }
    }

    vkDestroyShaderModule(owningDevice->logicalDevice, shaderModule, nullptr);
}

void Shader::LoadShader(const char *filepath, ShaderType shaderType, Device *device)
{
    name = File::GetFileNameFromPath(filepath);
    this->shaderType = shaderType;
    auto binaryCode = File::ReadBinaryFile(filepath);
    shaderModule = CreateShaderModule(binaryCode, device);
    shaderStage = CreateShaderStage(*this);

    std::cout << "Created " << ShaderTypeNames[static_cast<uint32_t>(shaderType)] << " shader '" << name << "' from " << filepath << '\n';
}

Shader Shader::CreateShaderFromFile(const char *filepath, ShaderType shaderType, Device *device)
{
    Shader shader(filepath, shaderType, device);
    return shader;
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &code, Device *device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    /* Reinterpret cast only works because std::vector allready uses worst case allignment for its data. */
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    VK_ASSERT(vkCreateShaderModule(device->logicalDevice, &createInfo, nullptr, &shaderModule), "Failed to create shader module!");

    return shaderModule;
}

VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(Shader &shader)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    switch (shader.shaderType)
    {
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

    if (!ShaderStages.contains(shader.owningDevice))
    {
        ShaderStages.insert(std::make_pair(shader.owningDevice, std::vector<VkPipelineShaderStageCreateInfo>{createInfo}));
    }
    else
    {
        ShaderStages.at(shader.owningDevice).push_back(createInfo);
    }

    return createInfo;
}