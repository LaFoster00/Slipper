#include "Mesh.h"

Mesh::Mesh(const Vertex *Vertices,
           const size_t NumVertices,
           const VertexIndex *Indices,
           const size_t NumIndices)
    : m_vertexBuffer(Vertices, NumVertices),
      m_indexBuffer(Indices, NumIndices)
{
}

void Mesh::Bind(const VkCommandBuffer &CommandBuffer) const
{
    const VkBuffer vertex_buffers[1] = {m_vertexBuffer};
    constexpr VkDeviceSize offsets[1] = {0};

    vkCmdBindVertexBuffers(CommandBuffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(CommandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}
