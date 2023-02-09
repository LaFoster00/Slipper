#pragma once

#include <array>
#include <vector>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Slipper
{
const std::string DEMO_MODEL_PATH = "./EngineContent/Models/VikingRoom/viking_room.obj";
const std::string DEMO_TEXTURE_PATH = "./EngineContent/Models/VikingRoom/viking_room.png";

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }

    static VkVertexInputBindingDescription *GetBindingDescription()
    {
        static VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return &binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 3> *GetAttributeDescriptions()
    {
        static std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

        return &attribute_descriptions;
    }
};
}  // namespace Slipper

namespace std
{
template<> struct hash<Slipper::Vertex>
{
    size_t operator()(Slipper::Vertex const &Vertex) const noexcept
    {
        return ((hash<glm::vec3>()(Vertex.pos) ^ (hash<glm::vec3>()(Vertex.color) << 1)) >> 1) ^
               (hash<glm::vec2>()(Vertex.texCoord) << 1);
    }
};
}  // namespace std

namespace Slipper
{
const std::vector<Vertex> DEBUG_TRIANGLE_VERTICES = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<VertexIndex> DEBUG_TRIANGLE_INDICES = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

class Mesh
{
 public:
    Mesh(const Vertex *Vertices,
         size_t NumVertices,
         const VertexIndex *Indices,
         size_t NumIndices);

    void Bind(const VkCommandBuffer &CommandBuffer) const;

    size_t NumVertex() const
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
}  // namespace Slipper