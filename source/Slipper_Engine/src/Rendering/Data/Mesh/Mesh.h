#pragma once

#include <array>

#include "common_includes.h"

#include <glm/glm.hpp>
#include <vector>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription *GetBindingDescription()
    {
        static VkVertexInputBindingDescription BindingDescription{};
        BindingDescription.binding = 0;
        BindingDescription.stride = sizeof(Vertex);
        BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return &BindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> *GetAttributeDescriptions()
    {
        static std::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions{};

        AttributeDescriptions[0].binding = 0;
        AttributeDescriptions[0].location = 0;
        AttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        AttributeDescriptions[0].offset = offsetof(Vertex, pos);

        AttributeDescriptions[1].binding = 0;
        AttributeDescriptions[1].location = 1;
        AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        AttributeDescriptions[1].offset = offsetof(Vertex, color);

        return &AttributeDescriptions;
    }
};

const std::vector<Vertex> DebugTriangleVertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                                   {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                                   {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<VertexIndex> DebugTriangleIndices = {0, 1, 2, 2, 3, 0};

class Mesh
{
 public:
    Mesh(CommandPool &MemoryCommandPool,
         const Vertex *Vertices,
         size_t NumVertices,
         const VertexIndex *Indices,
         size_t NumIndices);

    void Bind(const VkCommandBuffer &commandBuffer) const;

    size_t NumVertex()const
    {
        return vertexBuffer.numVertex;
    }

    size_t NumIndex() const
    {
        return indexBuffer.numIndex;
    }

 private:
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};