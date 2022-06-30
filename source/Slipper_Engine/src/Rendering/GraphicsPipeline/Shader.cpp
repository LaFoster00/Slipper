#include "Shader.h"

#include "../GraphicsEngine.h"
#include "../Setup/Device.h"
#include "Filesystem/File.h"
#include "Mesh/UniformBuffer.h"
#include "common_defines.h"

const char *ShaderTypeNames[]{"Vertex", "Fragment", "Compute"};

Shader::Shader(Device &device,
               GraphicsEngine *graphicsPipeline,
               std::string_view filepath,
               ShaderType shaderType,
               size_t uniformBufferCount,
               VkDeviceSize BufferSize,
               VkDescriptorSetLayout descriptorSetLayout)
    : device(device)
{
    this->graphicsPipeline = graphicsPipeline;

    LoadShader(filepath, shaderType);

    if (uniformBufferCount > 0) {
        CreateUniformBuffers(uniformBufferCount, BufferSize);
        CreateDescriptorPool(uniformBufferCount);
        CreateDescriptorSets(uniformBufferCount, descriptorSetLayout, BufferSize);
    }
}

void Shader::Destroy()
{
    if (descriptorPool)
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    uniformBuffers.clear();

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

std::vector<std::unique_ptr<UniformBuffer>> &Shader::CreateUniformBuffers(size_t count,
                                                                          VkDeviceSize BufferSize)
{
    uniformBuffers.reserve(count);
    for (int i = 0; i < count; ++i) {
        uniformBuffers.emplace_back(std::make_unique<UniformBuffer>(BufferSize));
    }

    return uniformBuffers;
}

void Shader::CreateDescriptorPool(size_t count)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(count);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(count);

    VK_ASSERT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool),
              "Failed to create descriptor pool!")
}

void Shader::CreateDescriptorSets(size_t count, VkDescriptorSetLayout descriptorSetLayout, VkDeviceSize bufferSize)
{
    std::vector<VkDescriptorSetLayout> layouts(count, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(count);
    VK_ASSERT(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()), "Failed to allocate descriptor sets!")

    for (int i = 0; i < count; ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;        // Optional
        descriptorWrite.pTexelBufferView = nullptr;  // Optional

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}

VkDescriptorSetLayout UniformMVP::GetDescriptorSetLayout()
{
    static VkDescriptorSetLayout layout = nullptr;
    if (!layout) {
        VkDescriptorSetLayoutBinding mvpLayoutBinding{};
        mvpLayoutBinding.binding = 0;
        mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpLayoutBinding.descriptorCount = 1;
        mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        mvpLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &mvpLayoutBinding;

        VK_ASSERT(vkCreateDescriptorSetLayout(Device::Get(), &layoutInfo, nullptr, &layout),
                  "Failed to create descriptor set layout!")
    }

    return layout;
}
