#include "VertexBuffer.h"

#include "Mesh.h"

namespace Slipper
{
VertexBuffer::VertexBuffer(const Vertex *Vertices, const size_t NumVertices)
    : Buffer(sizeof(Vertices[0]) * NumVertices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      numVertex(NumVertices)
{
    const Buffer staging_buffer(vkBufferSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    SetBufferData(Vertices, staging_buffer);
    // void *data;
    // vkMapMemory(device, stagingBuffer, 0, stagingBuffer.vkBufferSize, 0, &data);
    // memcpy(data, Vertices, (size_t)stagingBuffer.vkBufferSize);
    // vkUnmapMemory(device, stagingBuffer);

    Buffer::CopyBuffer(staging_buffer, *this);
}
}  // namespace Slipper