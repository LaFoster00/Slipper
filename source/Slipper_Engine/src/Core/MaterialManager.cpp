#include "MaterialManager.h"

#include "Material.h"

namespace Slipper
{
NonOwningPtr<Material> MaterialManager::AddMaterial(std::string Name, NonOwningPtr<Shader> Shader)
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

void MaterialManager::AddUniformUpdate(const Material &Material,
                                       const MaterialUniform UniformUpdate)
{
    auto &uniform_updates = m_uniformUpdates[&Material];
    auto &[frame_updated, uniform] = uniform_updates
                                         .try_emplace(&UniformUpdate.shaderBinding.get(),
                                                      UniformUpdateFunc{0, UniformUpdate})
                                         .first->second;
    frame_updated = Engine::FRAME_COUNT;
    uniform = UniformUpdate;
}

void MaterialManager::OnUpdate()
{
    for (auto &[material, binding_update] : m_uniformUpdates) {
        for (auto map = binding_update.begin(); map != binding_update.end();) {
            if (Engine::FRAME_COUNT - map->second.frame_updated >= Engine::MAX_FRAMES_IN_FLIGHT) {
                map = binding_update.erase(map);
            }
            else {
                material->BindUniformForThisFrame(map->second.uniform);
                ++map;
            }
        }
    }
}
}  // namespace Slipper
