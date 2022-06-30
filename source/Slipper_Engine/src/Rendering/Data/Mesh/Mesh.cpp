#include "Mesh.h"

#include "Setup/Device.h"

Mesh::Mesh(CommandPool &MemoryCommandPool,
           const Vertex *Vertices,
           size_t NumVertices,
           const VertexIndex *Indices,
           size_t NumIndices)
    : vertexBuffer(MemoryCommandPool, Vertices, NumVertices),
      indexBuffer(MemoryCommandPool, Indices, NumIndices)
{
}

void Mesh::Bind(const VkCommandBuffer &commandBuffer) const
{
    const VkBuffer vertexBuffers[1] = {vertexBuffer};
    const VkDeviceSize offsets[1] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}
