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

    virtual ~Buffer() noexcept;

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

    [[nodiscard]] std::optional<VkDescriptorImageInfo> GetDescriptorImageInfo() const override
    {
        return {};
    }

    [[nodiscard]] std::optional<VkDescriptorBufferInfo> GetDescriptorBufferInfo() const override
    {
        return VkDescriptorBufferInfo{vkBuffer, 0, vkBufferSize};
    }

    [[nodiscard]] constexpr VkDescriptorType GetDescriptorType() const override
    {
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const override
    {
        ASSERT(Binding.size == vkBufferSize,
               "Buffer size mismatch: Shader Binding -> {} | Supplied Buffer -> {}",
               Binding.size,
               vkBufferSize);
    }

 protected:
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize vkBufferSize;
};
}  // namespace Slipper