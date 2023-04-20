#pragma once
#include "AppComponent.h"
#include "Material.h"

namespace Slipper
{
struct DescriptorSetLayoutBinding;
struct MaterialUniform;
class Shader;
class Material;

class MaterialManager : public AppComponent
{
    friend Material;

 public:
    MaterialManager(const std::string_view &Name = "Material Manager") : AppComponent(Name)
    {
    }

    static NonOwningPtr<Material> AddMaterial(std::string Name, NonOwningPtr<Shader> Shader);
    static NonOwningPtr<Material> GetMaterial(std::string Name);
    static std::optional<NonOwningPtr<Material>> TryGetMaterial(std::string Name);

 private:
    struct UniformUpdateFunc
    {
        uint32_t frames_updated;
        MaterialUniform uniform;
    };

    static void AddUniformUpdate(const Material &Material, MaterialUniform UniformUpdate);

    void OnUpdate() override;

 private:
    static inline std::unordered_map<std::string, OwningPtr<Material>> m_materials;
    static inline std::unordered_map<
        NonOwningPtr<const Material>,
        std::unordered_map<NonOwningPtr<DescriptorSetLayoutBinding>, UniformUpdateFunc>>
        m_uniformUpdates;
};
}  // namespace Slipper