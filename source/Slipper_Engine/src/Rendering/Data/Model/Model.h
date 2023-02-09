#pragma once

#include <memory>

#include "Mesh/Mesh.h"

namespace Slipper
{
class Model
{
public:
    explicit Model(std::string_view FilePath);

    void Draw(VkCommandBuffer CommandBuffer, uint32_t InstanceCount = 1) const;
    
private:
    std::unique_ptr<Mesh> m_mesh;
};
}