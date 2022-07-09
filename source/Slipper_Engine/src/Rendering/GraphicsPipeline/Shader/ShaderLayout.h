#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "common_includes.h"
#include "DeviceDependentObject.h"
#include "Setup/Device.h"

class UniformBuffer;
class Texture;
class Buffer;
enum class ShaderMemberType : uint32_t;

struct ShaderMember
{
    std::string name;
    ShaderMemberType type;
    uint32_t size;
    uint32_t paddedSize;
    std::vector<ShaderMember> members;
};

struct DescriptorSetLayoutBinding
{
    std::string name;
    std::vector<ShaderMember> members;
    uint32_t size;
    uint32_t paddedSize;
    VkDescriptorSetLayoutBinding binding;
};

struct DescriptorSetLayoutInfo
{
    uint32_t setNumber;
    std::vector<DescriptorSetLayoutBinding> bindings;

    std::vector<VkDescriptorSetLayoutBinding> GetVkBindings() const;

    DescriptorSetLayoutInfo() = default;
    DescriptorSetLayoutInfo(const DescriptorSetLayoutInfo &Other) = delete;
    DescriptorSetLayoutInfo(DescriptorSetLayoutInfo &&Other) = delete;
};

class ShaderLayout : DeviceDependentObject
{
private:

 public :
    explicit ShaderLayout(const std::vector<char> &BinaryCode);
    ~ShaderLayout();
    void CreateDescriptorSetLayouts();
    void PopulateNamesLayoutBindings();
    void CreateDescriptorPool(uint32_t Count);
    void AllocateDescriptorSets(const uint32_t Count);

    bool SetBindingUniformBuffer(std::string Name, const UniformBuffer &Buffer) const;
    bool SetBindingTexture(std::string Name, const Texture &Texture) const;

public:
    VkDescriptorPool vkDescriptorPool;
    std::vector<VkDescriptorSetLayout> vkLayouts;
    std::vector<DescriptorSetLayoutInfo *> layoutInfos;

    std::unordered_map<VkDescriptorSetLayout, std::vector<VkDescriptorSet>> descriptorSets;
    std::unordered_map<std::string, DescriptorSetLayoutBinding *> namedLayoutBindings;
};
