#include "ShaderLayout.h"

#include <ranges>

#include "Mesh/UniformBuffer.h"
#include "ShaderReflection.h"
#include "Texture/Texture.h"
#include "common_defines.h"

std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutInfo::GetVkBindings() const
{
    std::vector<VkDescriptorSetLayoutBinding> vkBindings(bindings.size());
    for (int i = 0; i < vkBindings.size(); ++i) {
        vkBindings[i] = bindings[i].binding;
    }
    return vkBindings;
}

ShaderLayout::ShaderLayout(const std::vector<char> &BinaryCode) : vkDescriptorPool(nullptr)
{
    layoutInfos = ShaderReflection::CreateShaderBindingInfo(BinaryCode.data(), BinaryCode.size());

    CreateDescriptorSetLayouts();
    PopulateNamesLayoutBindings();
    CreateDescriptorPool(Engine::MAX_FRAMES_IN_FLIGHT);
    AllocateDescriptorSets(Engine::MAX_FRAMES_IN_FLIGHT);
}

ShaderLayout::~ShaderLayout()
{
    for (const auto vk_layout : vkLayouts) {
        vkDestroyDescriptorSetLayout(device, vk_layout, nullptr);
    }
    vkLayouts.clear();
}

void ShaderLayout::CreateDescriptorSetLayouts()
{
    vkLayouts.reserve(layoutInfos.size());
    for (int i = 0; i < layoutInfos.size(); ++i) {
        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        auto bindings = layoutInfos[i]->GetVkBindings();
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        createInfo.pBindings = bindings.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        vkCreateDescriptorSetLayout(Device::Get(), &createInfo, nullptr, &layout);
        vkLayouts.push_back(layout);
    }
}

void ShaderLayout::PopulateNamesLayoutBindings()
{
    for (const auto layout_info : layoutInfos) {
        for (auto &binding : layout_info->bindings) {
            namedLayoutBindings.insert(std::make_pair(String::to_lower(binding.name), &binding));
        }
    }
}

void ShaderLayout::CreateDescriptorPool(uint32_t Count)
{
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (const auto layout_info : layoutInfos) {
        for (const auto &binding : layout_info->bindings) {
            pool_sizes.push_back(VkDescriptorPoolSize{binding.binding.descriptorType,
                                                      binding.binding.descriptorCount});
        }
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = Count;

    VK_ASSERT(vkCreateDescriptorPool(device, &pool_info, nullptr, &vkDescriptorPool),
              "Failed to create descriptor pool!")
}

void ShaderLayout::AllocateDescriptorSets(const uint32_t Count)
{
    for (auto vk_layout : vkLayouts) {
        std::vector<VkDescriptorSetLayout> layouts(Count, vk_layout);

        VkDescriptorSetAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocate_info.descriptorPool = vkDescriptorPool;
        allocate_info.pSetLayouts = layouts.data();
        allocate_info.descriptorSetCount = static_cast<uint32_t>(layouts.size());

        descriptorSets[vk_layout].resize(layouts.size());
        VK_ASSERT(vkAllocateDescriptorSets(
                      Device::Get(), &allocate_info, descriptorSets.at(vk_layout).data()),
                  "Failed to allocate descriptor sets!");
    }
}

bool ShaderLayout::SetBindingUniformBuffer(const std::string Name,
                                           const UniformBuffer &Buffer) const
{
    if (!namedLayoutBindings.contains(String::to_lower(Name)))
        return false;

    const auto binding = namedLayoutBindings.at(String::to_lower(Name));
    ASSERT(binding->binding.descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
           "Descriptor '",
           Name,
           "' is not of type buffer");

    ASSERT(binding->size != Buffer.vkBufferSize,
           "Buffer '",
           Name,
           "' size missmatch! Shader expects ",
           std::to_string(binding->size),
           "bytes but buffer has ",
           std::to_string(Buffer.vkBufferSize),
           "bytes.");

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstBinding = binding->binding.binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = binding->binding.descriptorCount;
    descriptor_write.pBufferInfo = Buffer.GetDescriptorInfo();

    std::vector<VkWriteDescriptorSet> descriptor_writes;
    descriptor_writes.reserve(descriptorSets.size() * Engine::MAX_FRAMES_IN_FLIGHT);
    for (const auto &descriptor_sets : descriptorSets | std::views::values) {
        for (const auto descriptor_set : descriptor_sets) {
            VkWriteDescriptorSet set_write = descriptor_write;
            set_write.dstSet = descriptor_set;
            descriptor_writes.push_back(set_write);
        }
    }
    vkUpdateDescriptorSets(device,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(),
                           0,
                           nullptr);
    return true;
}

bool ShaderLayout::SetBindingTexture(const std::string Name, const Texture &Texture) const
{
    if (!namedLayoutBindings.contains(String::to_lower(Name)))
        return false;

    const auto binding = namedLayoutBindings.at(String::to_lower(Name));
    ASSERT(binding->binding.descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
           "Descriptor '",
           Name,
           "' is not of type combined image sampler");

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstBinding = binding->binding.binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_write.descriptorCount = binding->binding.descriptorCount;
    descriptor_write.pImageInfo = Texture.GetDescriptorImageInfo();

    std::vector<VkWriteDescriptorSet> descriptor_writes;
    descriptor_writes.reserve(descriptorSets.size() * Engine::MAX_FRAMES_IN_FLIGHT);
    for (const auto &descriptor_sets : descriptorSets | std::views::values) {
        for (const auto descriptor_set : descriptor_sets) {
            VkWriteDescriptorSet set_write = descriptor_write;
            set_write.dstSet = descriptor_set;
            descriptor_writes.push_back(set_write);
        }
    }
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
    return true;
}
