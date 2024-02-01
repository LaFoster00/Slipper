#include "Shader.h"

#include "GraphicsEngine.h"
#include "Buffer/UniformBuffer.h"

namespace Slipper
{
const char *ShaderTypeNames[]{"UNDEFINED", "Vertex", "Fragment", "Compute"};

Shader::~Shader()
{
    shaderLayout.reset();
    if (m_vkDescriptorPool)
        vkDestroyDescriptorPool(device, m_vkDescriptorPool, nullptr);

    for (const auto vk_descriptor_set_layout : m_vkDescriptorSetLayouts | std::views::values) {
        vkDestroyDescriptorSetLayout(device, vk_descriptor_set_layout, nullptr);
    }
    uniformBindingBuffers.clear();
}

UniformBuffer *Shader::GetUniformBuffer(const std::string Name,
                                        const std::optional<uint32_t> Index) const
{
    if (auto hash_binding = GetNamedBinding(Name); hash_binding.has_value()) {
        if (const Ref<DescriptorSetLayoutBinding> binding = hash_binding.value();
            uniformBindingBuffers.contains(GetHash(binding))) {

            return uniformBindingBuffers
                .at(GetHash(binding))[Index.has_value() ? Index.value() :
                                                          GraphicsEngine::Get().GetCurrentFrame()]
                .get();
        }
        ASSERT(false, "Uniform '{}' is not a buffer.", Name);
    }
    ASSERT(false, "Object '{}' does not exist.", Name);
}

std::vector<vk::DescriptorSet> Shader::GetDescriptorSets(const std::optional<uint32_t> Index) const
{
    std::vector<vk::DescriptorSet> ds;
    for (auto vk_descriptor_sets : m_vkDescriptorSets | std::ranges::views::values) {
        ds.push_back(
            vk_descriptor_sets[Index.has_value() ? Index.value() :
                                                   GraphicsEngine::Get().GetCurrentFrame()]);
    }
    return ds;
}

void Shader::CreateDescriptorPool()
{
    std::vector<vk::DescriptorPoolSize> pool_sizes;
    for (const auto set_layout : shaderLayout->setLayouts) {
        for (DescriptorSetLayoutBinding binding : set_layout.bindings) {
            /* Need to multiply by Engine::MAX_FRAMES_IN_FLIGHT since vulkan needs to know the
             * total amount of descriptors as well as the total amount of sets these descriptors
             * divide into */
            pool_sizes.push_back(
                {binding.descriptorType, binding.descriptorCount * Engine::MAX_FRAMES_IN_FLIGHT});
        }
    }

    vk::DescriptorPoolCreateInfo pool_info(vk::DescriptorPoolCreateFlags{},
                                           shaderLayout->setLayouts.size() *
                                               Engine::MAX_FRAMES_IN_FLIGHT,
                                           pool_sizes);

    VK_HPP_ASSERT(
        device.logicalDevice.createDescriptorPool(&pool_info, nullptr, &m_vkDescriptorPool),
        "Failed to create descriptor pool!")
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &Code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = Code.size();
    /* Reinterpret cast only works because std::vector allready uses worst case allignment for its
     * data. */
    create_info.pCode = reinterpret_cast<const uint32_t *>(Code.data());

    VkShaderModule shader_module;
    VK_ASSERT(vkCreateShaderModule(VKDevice::Get(), &create_info, nullptr, &shader_module),
              "Failed to create shader module!");

    return shader_module;
}

VkPipelineShaderStageCreateInfo Shader::CreateShaderStage(const ShaderType &ShaderType,
                                                          const VkShaderModule &ShaderModule)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    switch (ShaderType) {
        case ShaderType::VERTEX:
            createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::FRAGMENT:
            createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::COMPUTE:
            createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
    }

    createInfo.module = ShaderModule;
    createInfo.pName = "main";

    return createInfo;
}

void Shader::CreateUniformBuffers()
{
    for (const auto layout_binding : shaderLayout->namedLayoutBindings | std::views::values) {
        if (layout_binding->descriptorType == vk::DescriptorType::eUniformBuffer) {
            auto &buffers = uniformBindingBuffers[GetHash(layout_binding)];
            for (int i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; ++i) {
                auto &new_buffer = buffers.emplace_back(new UniformBuffer(layout_binding->size));
                BindShaderUniform(layout_binding->name, *new_buffer, i);
            }
        }
    }
}

void Shader::CreateDescriptorSetLayouts()
{
    for (auto set_layout : shaderLayout->setLayouts) {
        m_vkDescriptorSetLayouts[set_layout.setNumber] =
            device.logicalDevice.createDescriptorSetLayout(set_layout.createInfo);
    }
}

void Shader::AllocateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve(m_vkDescriptorSetLayouts.size() * Engine::MAX_FRAMES_IN_FLIGHT);

    // Make a duplicate of each descriptor set layout for each frame so that they can be set on a
    // per frame basis
    for (auto vk_descriptor_set_layout : m_vkDescriptorSetLayouts | std::ranges::views::values) {
        std::fill_n(
            std::back_inserter(layouts), Engine::MAX_FRAMES_IN_FLIGHT, vk_descriptor_set_layout);
    }

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(layouts.size());

    VkDescriptorSetAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = m_vkDescriptorPool;
    allocate_info.pSetLayouts = layouts.data();
    allocate_info.descriptorSetCount = layouts.size();

    VK_ASSERT(vkAllocateDescriptorSets(VKDevice::Get(), &allocate_info, descriptor_sets.data()),
              "Failed to allocate descriptor sets!");
    uint32_t i = 0;
    for (const auto &set_number : m_vkDescriptorSetLayouts | std::views::keys) {
        auto &set_layout = m_vkDescriptorSets[set_number];
        auto begin_offset_it = descriptor_sets.begin();
        std::advance(begin_offset_it, i * Engine::MAX_FRAMES_IN_FLIGHT);
        auto end_offset_it = descriptor_sets.begin();
        std::advance(end_offset_it,
                     i * Engine::MAX_FRAMES_IN_FLIGHT + Engine::MAX_FRAMES_IN_FLIGHT);
        set_layout.insert(set_layout.end(), begin_offset_it, end_offset_it);

        i++;
    }
}

void Shader::BindShaderUniform_Interface(const DescriptorSetLayoutBindingMinimal &Binding,
                                         const IShaderBindableData &Object,
                                         std::optional<uint32_t> Index) const
{
    vk::WriteDescriptorSet descriptor_write(
        {} /*Set Later*/, Binding.binding, 0, Object.GetDescriptorType(), {}, {}, {});

    const auto buffer_info = Object.GetDescriptorBufferInfo();
    if (buffer_info.has_value()) {
        descriptor_write.setBufferInfo(buffer_info.value());
    }

    const auto image_info = Object.GetDescriptorImageInfo();
    if (image_info.has_value()) {
        descriptor_write.setImageInfo(image_info.value());
    }

    UpdateDescriptorSets(descriptor_write, Binding, Index);
}

void Shader::UpdateDescriptorSets(vk::WriteDescriptorSet DescriptorWrite,
                                  const DescriptorSetLayoutBindingMinimal &Binding,
                                  std::optional<uint32_t> Index) const
{
    if (Index.has_value()) {
        DescriptorWrite.setDstSet(m_vkDescriptorSets.at(Binding.set)[Index.value()]);
        device.logicalDevice.updateDescriptorSets(DescriptorWrite, {});
    }
    else {
        // Update all descriptor sets at once
        std::vector<vk::WriteDescriptorSet> descriptor_writes;
        descriptor_writes.reserve(Engine::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t frame = 0; frame < Engine::MAX_FRAMES_IN_FLIGHT; ++frame) {
            DescriptorWrite.dstSet = m_vkDescriptorSets.at(Binding.set)[frame];
            descriptor_writes.push_back(DescriptorWrite);
        }
        device.logicalDevice.updateDescriptorSets(descriptor_writes, {});
    }
}
}  // namespace Slipper