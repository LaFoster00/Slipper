#pragma once

namespace Slipper
{
struct DescriptorSetLayoutBinding;

class IShaderBindableData
{
 public:
    [[nodiscard]] virtual std::optional<VkDescriptorImageInfo> GetDescriptorImageInfo() const = 0;
    [[nodiscard]] virtual std::optional<VkDescriptorBufferInfo> GetDescriptorBufferInfo()
        const = 0;
    [[nodiscard]] virtual constexpr VkDescriptorType GetDescriptorType() const = 0;
    virtual void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const = 0;
};
}  // namespace Slipper