#pragma once

#include "common_includes.h"

#include "Buffer.h"

class CommandPool;

namespace Mesh
{
	struct Vertex;
}

class Device;

class VertexBuffer : public Buffer
{
 public:
    VertexBuffer(Device &Device, CommandPool &MemoryCommandPool, const Mesh::Vertex *Vertices, size_t NumVertices);
    ~VertexBuffer();

    void SubmitVertexBuffer(CommandPool &MemoryCommandPool);

 public:
    Device &device;

    size_t numVertex;
};
