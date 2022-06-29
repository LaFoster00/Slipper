#include "VertexBuffer.h"

#include "Mesh.h"
#include "Setup/Device.h"

#include "common_defines.h"

VertexBuffer::VertexBuffer(Device &Device, const Mesh::Vertex *Vertices, size_t NumVertices)
    : device(Device), numVertices(NumVertices)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertices[0]) * NumVertices;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &vkVertexBuffer),
              "Failed to create vertex buffer!")

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vkVertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits,
                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &vkVertexBufferMemory),
              "Failed to allocate vertex buffer memory!")
    vkBindBufferMemory(device, vkVertexBuffer, vkVertexBufferMemory, 0);

    void* data;
    vkMapMemory(device, vkVertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, Vertices, bufferInfo.size);
    vkUnmapMemory(device, vkVertexBufferMemory);
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(device, vkVertexBuffer, nullptr);
    vkFreeMemory(device, vkVertexBufferMemory, nullptr);
}

VertexBuffer::operator VkBuffer() const
{
    return vkVertexBuffer;
}
