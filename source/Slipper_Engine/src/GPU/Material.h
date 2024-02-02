#pragma once
#include "Vulkan/vk_GraphicsShader.h"
#include "Vulkan/vk_Material.h"
#include "Vulkan/vk_Texture2D.h"

namespace Slipper::GPU
{
    class Material : Object<Material>
    {
     public:
        explicit Material(NonOwningPtr<Vulkan::GraphicsShader> material);
        void BindUniformForThisFrame(Vulkan::MaterialUniform uniform) const;
        void SetUniform(const char * str, const Vulkan::Texture2D & texture_2d);
    };
}  // namespace Slipper::GPU
