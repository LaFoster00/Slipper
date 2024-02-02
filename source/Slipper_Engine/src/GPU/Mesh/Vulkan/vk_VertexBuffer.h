#pragma once

namespace Slipper::GPU::Vulkan
{
    struct Vertex;

    class VertexBuffer : public Buffer
    {
     public:
        VertexBuffer(const Vertex *Vertices, size_t NumVertices);

     public:
        size_t numVertex;
    };
}  // namespace Slipper::GPU::Vulkan
