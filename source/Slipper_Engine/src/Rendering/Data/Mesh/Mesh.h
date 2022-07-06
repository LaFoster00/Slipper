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
        static VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return &binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> *GetAttributeDescriptions()
    {
        static std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        return &attribute_descriptions;
    }
};

const std::vector<Vertex> DEBUG_TRIANGLE_VERTICES = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                                   {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                                   {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<VertexIndex> DEBUG_TRIANGLE_INDICES = {0, 1, 2, 2, 3, 0};

class Mesh
{
 public:
    Mesh(const Vertex *Vertices,
         size_t NumVertices,
         const VertexIndex *Indices,
         size_t NumIndices);

    void Bind(const VkCommandBuffer &CommandBuffer) const;

    size_t NumVertex()const
    {
        return m_vertexBuffer.numVertex;
    }

    size_t NumIndex() const
    {
        return m_indexBuffer.numIndex;
    }

 private:
    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;
};