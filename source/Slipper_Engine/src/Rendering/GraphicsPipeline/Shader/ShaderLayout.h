#pragma once
#include "common_defines.h"

namespace Slipper
{
class UniformBuffer;
class Texture;
class Buffer;
enum class ShaderMemberType : uint32_t;

enum ShaderResourceType
{
    SPV_REFLECT_RESOURCE_FLAG_UNDEFINED = 0x00000000,
    SPV_REFLECT_RESOURCE_FLAG_SAMPLER = 0x00000001,
    SPV_REFLECT_RESOURCE_FLAG_CBV = 0x00000002,
    SPV_REFLECT_RESOURCE_FLAG_SRV = 0x00000004,
    SPV_REFLECT_RESOURCE_FLAG_UAV = 0x00000008,
};

struct DescriptorSetLayoutBinding
{
    // VkDescriptorSetLayoutBinding
    // Dont change layout of this since it will be referenced when converting to vulkan object with
    // VkDescriptorSetLayoutBinding()
    uint32_t binding;
    VkDescriptorType descriptorType;
    uint32_t descriptorCount;
    VkShaderStageFlags stageFlags;
    const VkSampler *pImmutableSamplers;
    // End of Vulkan Object

    // User Information
    uint32_t spirvId;
    std::string name;

    // Binding Information
    uint32_t inputAttachmentIndex;
    uint32_t set;
    ShaderResourceType resourceType;

    // Memory Information
    uint32_t offset;          // Measured in bytes
    uint32_t absoluteOffset;  // Measured in bytes
    uint32_t size;            // Measured in bytes
    uint32_t paddedSize;      // Measured in bytes

    VkDescriptorSetLayoutBinding &GetVkBinding()
    {
        return *reinterpret_cast<VkDescriptorSetLayoutBinding *>(&binding);
    }

    const VkDescriptorSetLayoutBinding &GetVkBinding() const
    {
        return *reinterpret_cast<const VkDescriptorSetLayoutBinding *>(&binding);
    }
};
}  // namespace Slipper

namespace std
{
template<> struct hash<Slipper::DescriptorSetLayoutBinding>
{
    size_t operator()(Slipper::DescriptorSetLayoutBinding const &binding) const noexcept
    {
        size_t hash = 0;
        hash_combine(hash,
                     binding.binding,
                     binding.descriptorType,
                     binding.descriptorCount,
                     binding.stageFlags,
                     binding.name,
                     binding.set,
                     binding.resourceType);
        return hash;  // or use boost::hash_combine (see Discussion)
                                // https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};
}  // namespace std

namespace Slipper
{
struct DescriptorSetLayoutData
{
    uint32_t setNumber;
    VkDescriptorSetLayoutCreateInfo createInfo;
    std::vector<DescriptorSetLayoutBinding> bindings;
    // Required for the create info. Otherwise it will reference an invalid array
    std::vector<VkDescriptorSetLayoutBinding> vkBindings;
};

class ShaderModuleLayout : DeviceDependentObject
{
 public:
    explicit ShaderModuleLayout(const std::vector<char> &BinaryCode);
    ShaderModuleLayout(const ShaderModuleLayout &Other) : setLayouts(Other.setLayouts)
    {
        PopulateNamesLayoutBindings();
    }

    ShaderModuleLayout(const ShaderModuleLayout &&Other) noexcept
        : setLayouts(std::move(Other.setLayouts))
    {
        PopulateNamesLayoutBindings();
    }

 private:
    void PopulateNamesLayoutBindings();

 public:
    std::vector<DescriptorSetLayoutData> setLayouts;
    std::unordered_map<std::string, DescriptorSetLayoutBinding *> namedLayoutBindings;
};
}  // namespace Slipper