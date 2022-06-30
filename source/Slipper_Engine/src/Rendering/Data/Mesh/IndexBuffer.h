#pragma once
#include "Buffer.h"

typedef uint16_t VertexIndex;

class IndexBuffer : public Buffer
{
public:
    IndexBuffer(CommandPool &MemoryCommandPool, const VertexIndex *Indices, size_t NumIndices);

public:
    size_t numIndex;
};