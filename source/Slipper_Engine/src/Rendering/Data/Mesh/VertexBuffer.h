#pragma once

#include "Buffer.h"

struct Vertex;
class CommandPool;

class Device;

class VertexBuffer : public Buffer
{
 public:
    VertexBuffer(const Vertex *Vertices, size_t NumVertices);

 public:
    size_t numVertex;
};