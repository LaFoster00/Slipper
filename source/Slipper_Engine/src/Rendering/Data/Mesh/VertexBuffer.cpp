#include "VertexBuffer.h"

#include "Mesh.h"
#include "Setup/Device.h"

#include "Drawing/CommandPool.h"

VertexBuffer::VertexBuffer(CommandPool &MemoryCommandPool,
                           const Vertex *Vertices,
                           const size_t NumVertices)
    : Buffer(Device::Get(),
             sizeof(Vertices[0]) * NumVertices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      numVertex(NumVertices)
{
    const Buffer staging_buffer(device,
                               vkBufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    SetBufferData(Vertices, staging_buffer);
    // void *data;
    // vkMapMemory(device, stagingBuffer, 0, stagingBuffer.vkBufferSize, 0, &data);
    // memcpy(data, Vertices, (size_t)stagingBuffer.vkBufferSize);
    // vkUnmapMemory(device, stagingBuffer);

    Buffer::CopyBuffer(device, MemoryCommandPool, staging_buffer, *this);
}