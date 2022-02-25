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
class GraphicsEngine;

class Shader
{
public:
    Shader() = delete;
    Shader(Device &device, GraphicsEngine *graphicsPipeline, const char *filepath, ShaderType shaderType);
    void Destroy();

    static VkShaderModule CreateShaderModule(const std::vector<char> &code, Device &device);
    static VkPipelineShaderStageCreateInfo CreateShaderStage(Shader &shader);

private:
    void LoadShader(const char *filepath, ShaderType shaderType);

public:
    Device &device;
    GraphicsEngine *graphicsPipeline;

    std::string name;
    ShaderType shaderType;
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo shaderStage;
};
