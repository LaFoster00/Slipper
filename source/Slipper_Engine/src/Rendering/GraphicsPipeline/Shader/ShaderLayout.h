#pragma once

#include <memory>
#include <string>
#include <unordered_map>

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
    uint32_t setNumber;
    std::vector<ShaderMember> members;
    uint32_t size;
    uint32_t paddedSize;
    VkDescriptorSetLayoutBinding binding;
};

struct ModuleDescriptorSetLayoutInfo
{
    std::vector<DescriptorSetLayoutBinding> bindings;

    std::vector<VkDescriptorSetLayoutBinding> GetVkBindings() const;

    ModuleDescriptorSetLayoutInfo() = default;
    ModuleDescriptorSetLayoutInfo(const ModuleDescriptorSetLayoutInfo &Other) = delete;
    ModuleDescriptorSetLayoutInfo(ModuleDescriptorSetLayoutInfo &&Other) = delete;
};

class ShaderModuleLayout : DeviceDependentObject
{
private:

 public :
    explicit ShaderModuleLayout(const std::vector<char> &BinaryCode);
    ~ShaderModuleLayout();
    void PopulateNamesLayoutBindings();

public:
    std::unique_ptr<ModuleDescriptorSetLayoutInfo> layoutInfo;
    std::unordered_map<std::string, DescriptorSetLayoutBinding *> namedLayoutBindings;
};
