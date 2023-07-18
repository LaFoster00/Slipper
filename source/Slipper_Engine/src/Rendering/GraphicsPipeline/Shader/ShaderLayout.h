#pragma once
#include "common_defines.h"

namespace Slipper
{
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
    vk::DescriptorType descriptorType;
    uint32_t descriptorCount;
    vk::ShaderStageFlags stageFlags;
    const vk::Sampler *pImmutableSamplers;
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

    vk::DescriptorSetLayoutBinding &GetVkBinding()
    {
        return *reinterpret_cast<vk::DescriptorSetLayoutBinding *>(&binding);
    }

    const vk::DescriptorSetLayoutBinding &GetVkBinding() const
    {
        return *reinterpret_cast<const vk::DescriptorSetLayoutBinding *>(&binding);
    }

    using HashT = size_t;
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
                     static_cast<VkFlags>(binding.stageFlags),
                     binding.name,
                     binding.set,
                     binding.resourceType);
        return hash;  // or use boost::hash_combine (see Discussion)
                      // https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};
}  // namespace std

inline size_t GetHash(const Slipper::DescriptorSetLayoutBinding &Object)
{
    return std::hash<Slipper::DescriptorSetLayoutBinding>{}(Object);
}

inline size_t GetHash(const Slipper::DescriptorSetLayoutBinding *Object)
{
    return std::hash<Slipper::DescriptorSetLayoutBinding>{}(*Object);
}

namespace Slipper
{
struct DescriptorSetLayoutData
{
    uint32_t setNumber;
    vk::DescriptorSetLayoutCreateInfo createInfo;
    std::vector<DescriptorSetLayoutBinding> bindings;
    // Required for the create info. Otherwise it will reference an invalid array
    std::vector<vk::DescriptorSetLayoutBinding> vkBindings;

    DescriptorSetLayoutData() = default;

    // Only move otherwise create info wont be valid
    DescriptorSetLayoutData(const DescriptorSetLayoutData &&Other) noexcept
	    : setNumber(Other.setNumber),
	      createInfo(Other.createInfo),
	      bindings(Other.bindings),
	      vkBindings(Other.vkBindings)
    {
        createInfo.pBindings = vkBindings.data();
    }
    DescriptorSetLayoutData(const DescriptorSetLayoutData &Other)
        : setNumber(Other.setNumber),
          createInfo(Other.createInfo),
          bindings(Other.bindings),
          vkBindings(Other.vkBindings)
    {
        createInfo.pBindings = vkBindings.data();
    };

    void UpdateCreateInfo()
    {
        vkBindings.clear();
        vkBindings.reserve(bindings.size());
	    for (auto &binding : bindings)
	    {
            vkBindings.push_back(binding.GetVkBinding());
	    }
        createInfo.pBindings = vkBindings.data();
        createInfo.bindingCount = vkBindings.size();
    }
};

class ShaderLayout : DeviceDependentObject
{
 public:
    explicit ShaderLayout(const std::vector<std::vector<char>> &BinaryCodes);
    ShaderLayout(const ShaderLayout &Other) : setLayouts(Other.setLayouts)
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