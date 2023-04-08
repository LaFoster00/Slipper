#pragma once

#include "Buffer/Buffer.h"

namespace Slipper
{
struct Vertex;
class CommandPool;

class VertexBuffer : public Buffer
{
 public:
    VertexBuffer(const Vertex *Vertices, size_t NumVertices);

 public:
    size_t numVertex;
};
}  // namespace Slipper
