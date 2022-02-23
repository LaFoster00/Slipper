#pragma once

#include "common_includes.h"
#include "common_defines.h"

#include <vector>
#include <string>

// DO NOT CHANGE NUMBERS WITHOUT CHANGING SHADERTYPENAMES ARRAY!!!
enum class ShaderType
{
    Vertex = 0,
    Fragment = 1,
    Compute = 2
};

extern const char *ShaderTypeNames[];

class Device;

class Shader
{
public:
    Shader();
    Shader(const char *filepath, ShaderType shaderType, Device *device);
    void Destroy();

    void LoadShader(const char *filepath, ShaderType shaderType, Device *device);

    static Shader CreateShaderFromFile(const char *filepath, ShaderType shaderType, Device *device);
    static VkShaderModule CreateShaderModule(const std::vector<char> &code, Device *device);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(Shader &shader);

public:
    Device *owningDevice;

    std::string name;
    ShaderType shaderType;
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo shaderStage;
};
