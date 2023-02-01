#include "Model.h"

#include "Filesystem/Path.h"
#include "tiny_obj_loader.h"

Model::Model(std::string_view FilePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib,
                          &shapes,
                          &materials,
                          &warn,
                          &err,
                          Path::make_engine_relative_path_absolute(DEMO_MODEL_PATH).c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                          attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    m_mesh = std::make_unique<Mesh>(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void Model::Draw(VkCommandBuffer CommandBuffer, uint32_t InstanceCount) const
{
    m_mesh->Bind(CommandBuffer);
    vkCmdDrawIndexed(CommandBuffer, m_mesh->NumIndex(), InstanceCount, 0, 0, 0);
}
