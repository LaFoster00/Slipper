#pragma once

#include "common_includes.h"

namespace Mesh
{
	struct Vertex;
}

class Device;

class VertexBuffer
{
 public:
    VertexBuffer(Device &Device, const Mesh::Vertex *Vertices, size_t NumVertices);
    ~VertexBuffer();

    operator VkBuffer() const;

 public:
    VkBuffer vkVertexBuffer;
    VkDeviceMemory vkVertexBufferMemory;

    Device &device;

    size_t numVertices;
};
