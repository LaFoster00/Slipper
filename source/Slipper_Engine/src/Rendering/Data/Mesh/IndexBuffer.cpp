#include "IndexBuffer.h"

#include "Setup/Device.h"

IndexBuffer::IndexBuffer(CommandPool &MemoryCommandPool,
                         const VertexIndex *Indices,
                         const size_t NumIndices)
    : Buffer(Device::Get(),
             sizeof(Indices[0]) * NumIndices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      numIndex(NumIndices)
{
    const Buffer staging_buffer(Device::Get(),
                               vkBufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    SetBufferData(Indices, staging_buffer);

    Buffer::CopyBuffer(Device::Get(), MemoryCommandPool, staging_buffer, *this);
}
