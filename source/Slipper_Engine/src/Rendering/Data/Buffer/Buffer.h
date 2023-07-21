#pragma once
#include "IShaderBindableData.h"
#include "Shader/ShaderLayout.h"

namespace Slipper
{
struct ShaderUniformObject;
class CommandPool;

class Buffer : DeviceDependentObject, public IShaderBindableData
{
 public:
    Buffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);

    ~Buffer() noexcept override;

    Buffer(Buffer &&Source) noexcept;

    /* Sets the buffers data to that of the supplied pointer based on the size specified during
     * creation. */
    template<typename TDataObject>
    static void SetBufferData(const TDataObject DataObject, const Buffer &Buffer)
        requires std::is_pointer_v<TDataObject>
    {
        void *data;
        vkMapMemory(Device::Get(), Buffer, 0, Buffer.vkBufferSize, 0, &data);
        memcpy(data, DataObject, static_cast<size_t>(Buffer.vkBufferSize));
        vkUnmapMemory(Device::Get(), Buffer);
    }

    static void SetBufferData(const ShaderUniformObject *DataObject, const Buffer &Buffer);
    static void CopyBuffer(const Buffer &SrcBuffer, const Buffer &DstBuffer);

    void SetBufferData(const ShaderUniformObject *DataObject) const
    {
        SetBufferData(DataObject, *this);
    }

    void CopyBuffer(const Buffer &SrcBuffer) const
    {
        CopyBuffer(SrcBuffer, *this);
    }

    operator VkBuffer() const
    {
        return vkBuffer;
    }

    operator VkDeviceMemory() const
    {
        return vkBufferMemory;
    }

    operator vk::Buffer() const
    {
        return vkBuffer;
    }

    [[nodiscard]] std::optional<vk::DescriptorImageInfo> GetDescriptorImageInfo() const override
    {
        return {};
    }

    [[nodiscard]] std::optional<vk::DescriptorBufferInfo> GetDescriptorBufferInfo() const override
    {
        return vk::DescriptorBufferInfo(vkBuffer, 0, vkBufferSize);
    }

    [[nodiscard]] constexpr vk::DescriptorType GetDescriptorType() const override
    {
        return vk::DescriptorType::eStorageBuffer;
    }

    void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const override
    {
        ASSERT(Binding.size <= vkBufferSize,
               "Buffer size mismatch: Shader Binding -> {} | Supplied Buffer -> {}",
               Binding.size,
               vkBufferSize);
    }

    void AdditionalBindingChecks(const DescriptorSetLayoutBindingMinimal &Binding) const override
    {
        LOG("Binding and object directly to a shader is dangerous and doesnt provide any extra "
            "safety checks. Please avoid.")
    }

 protected:
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize vkBufferSize;
};
}  // namespace Slipper