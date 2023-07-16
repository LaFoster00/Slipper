#pragma once

namespace Slipper
{
struct DescriptorSetLayoutBinding;

class IShaderBindableData
{
 public:
    [[nodiscard]] virtual std::optional<vk::DescriptorImageInfo> GetDescriptorImageInfo()
        const = 0;
    [[nodiscard]] virtual std::optional<vk::DescriptorBufferInfo> GetDescriptorBufferInfo()
        const = 0;
    [[nodiscard]] virtual constexpr vk::DescriptorType GetDescriptorType() const = 0;
    virtual void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const = 0;
};
}  // namespace Slipper