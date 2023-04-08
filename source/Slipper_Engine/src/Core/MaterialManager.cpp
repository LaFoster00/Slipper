#include "MaterialManager.h"

#include "Material.h"

namespace Slipper
{
NonOwningPtr<Material> MaterialManager::AddMaterial(std::string Name,
                                                    NonOwningPtr<Shader> Shader)
{
    if (m_materials.contains(Name)) {
        LOG_FORMAT(
            "Material with name '{}' already exists. Please use a different name or delete the "
            "old material beforehand.",
            Name);
        return nullptr;
    }
    return m_materials.emplace(Name, new Material(Shader)).first->second;
}

NonOwningPtr<Material> MaterialManager::GetMaterial(const std::string Name)
{
    return m_materials.at(Name);
}

std::optional<NonOwningPtr<Material>> MaterialManager::TryGetMaterial(const std::string Name)
{
    if (!m_materials.contains(Name)) {
        return {};
    }
    return m_materials.at(Name).get();
}
}  // namespace Slipper
