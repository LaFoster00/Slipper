#pragma once

namespace Slipper
{
	class Shader;
	class Material;

class MaterialManager
{
 public:
    static NonOwningPtr<Material> AddMaterial(std::string Name, NonOwningPtr<Shader> Shader);
    static NonOwningPtr<Material> GetMaterial(std::string Name);
    static std::optional<NonOwningPtr<Material>> TryGetMaterial(std::string Name);

 private:
    static inline std::unordered_map<std::string, OwningPtr<Material>> m_materials;
};
}  // namespace Slipper