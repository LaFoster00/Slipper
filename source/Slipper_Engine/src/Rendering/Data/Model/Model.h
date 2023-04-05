#pragma once

#include <memory>

#include "Mesh/Mesh.h"
#include "Shader/Shader.h"

namespace Slipper
{
struct UniformModel : ShaderUniformObject
{
    glm::mat4 model = {};
	size_t GetDataSize() const override
	{
        return sizeof(UniformModel);
	}

	void const* GetData() const override
	{
        return &model;
	}
	
};
class Model
{
 public:
    explicit Model(std::string_view FilePath);

    void Draw(VkCommandBuffer CommandBuffer, uint32_t InstanceCount = 1) const;
	const Mesh &GetMesh() const
    {
        return *m_mesh;
    }

private:
    std::unique_ptr<Mesh> m_mesh;
};
}  // namespace Slipper
