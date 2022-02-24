#pragma once

#include "common_includes.h"

#include <vector>
#include <string>
#include <unordered_map>

// DO NOT CHANGE NUMBERS WITHOUT CHANGING SHADERTYPENAMES ARRAY!!!
enum class ShaderType
{
    Vertex = 0,
    Fragment = 1,
    Compute = 2
};

extern const char *ShaderTypeNames[];

class Device;
class GraphicsPipeline;

class Shader
{
public:
    Shader();
    Shader(const char *filepath, ShaderType shaderType, Device *device, GraphicsPipeline *graphicsPipeline);
    void Destroy();

    void LoadShader(const char *filepath, ShaderType shaderType, Device *device, GraphicsPipeline *graphicsPipeline);

    static Shader CreateShaderFromFile(const char *filepath, ShaderType shaderType, Device *device, GraphicsPipeline *graphicsPipeline);
    static VkShaderModule CreateShaderModule(const std::vector<char> &code, Device *device);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(Shader &shader);

public:
    Device *owningDevice;
    GraphicsPipeline *owningGraphicsPipeline;

    std::string name;
    ShaderType shaderType;
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo shaderStage;
};
