#include "IndexBuffer.h"

#include "Setup/Device.h"

IndexBuffer::IndexBuffer(CommandPool &MemoryCommandPool,
                         const VertexIndex *Indices,
                         size_t NumIndices)
    : Buffer(Device::Get(),
             sizeof(Indices[0]) * NumIndices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      numIndex(NumIndices)
{
    const Buffer stagingBuffer(Device::Get(),
                               vkBufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(device, stagingBuffer, 0, stagingBuffer.vkBufferSize, 0, &data);
    memcpy(data, Indices, (size_t)stagingBuffer.vkBufferSize);
    vkUnmapMemory(device, stagingBuffer);

    Buffer::CopyBuffer(Device::Get(), MemoryCommandPool, stagingBuffer, *this);
}
