#include "IndexBuffer.h"

namespace Slipper
{
IndexBuffer::IndexBuffer(const VertexIndex *Indices,
                         const size_t NumIndices)
    : Buffer(sizeof(Indices[0]) * NumIndices,
             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      numIndex(NumIndices)
{
	const Buffer staging_buffer(vkBufferSize,
	                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    SetBufferData(Indices, staging_buffer);

    Buffer::CopyBuffer(staging_buffer, *this);
}
}