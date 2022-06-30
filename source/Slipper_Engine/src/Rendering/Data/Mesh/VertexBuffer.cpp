#include "VertexBuffer.h"

#include "Mesh.h"
#include "Setup/Device.h"

#include "Drawing/CommandPool.h"
#include "common_defines.h"

VertexBuffer::VertexBuffer(Device &Device,
                           CommandPool &MemoryCommandPool,
                           const Mesh::Vertex *Vertices,
                           size_t NumVertices)
    : Buffer(Device,
             sizeof(Vertices[0]) * NumVertices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      device(Device),
      numVertex(NumVertices)
{
    const Buffer stagingBuffer(Device,
                               vkBufferSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(device, stagingBuffer, 0, stagingBuffer.vkBufferSize, 0, &data);
    memcpy(data, Vertices, (size_t)stagingBuffer.vkBufferSize);
    vkUnmapMemory(device, stagingBuffer);

    Buffer::CopyBuffer(device, MemoryCommandPool, stagingBuffer, *this);
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::SubmitVertexBuffer(CommandPool &MemoryCommandPool)
{
}
