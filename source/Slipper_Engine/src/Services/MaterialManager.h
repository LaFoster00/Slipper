#pragma once
#include "AppComponent.h"
#include "Material.h"
#include "Vulkan/vk_GraphicsShader.h"
#include "Vulkan/vk_Material.h"

namespace Slipper
{
    class MaterialManager : public AppComponent
    {
        friend GPU::Material;

     public:
        MaterialManager(const std::string_view &Name = "Material Manager") : AppComponent(Name)
        {
        }

        static NonOwningPtr<GPU::Material> AddMaterial(std::string Name,
                                                       NonOwningPtr<GPU::Vulkan::GraphicsShader> Shader);
        static NonOwningPtr<GPU::Material> GetMaterial(std::string Name);
        static std::optional<NonOwningPtr<GPU::Material>> TryGetMaterial(std::string Name);

     private:
        struct UniformUpdateFunc
        {
            uint32_t frames_updated;
            GPU::Vulkan::MaterialUniform uniform;
        };

        static void AddUniformUpdate(const GPU::Material &Material, GPU::Vulkan::MaterialUniform UniformUpdate);

        void OnUpdate() override;

     private:
        static inline std::unordered_map<std::string, OwningPtr<GPU::Material>> m_materials;
        static inline std::unordered_map<
            NonOwningPtr<const GPU::Material>,
            std::unordered_map<NonOwningPtr<GPU::Vulkan::DescriptorSetLayoutBinding>, UniformUpdateFunc>>
            m_uniformUpdates;
    };
}  // namespace Slipper
