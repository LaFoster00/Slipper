#include "ShaderManager.h"

#include "File.h"

namespace Slipper
{
NonOwningPtr<Shader> ShaderManager::GetShader(const std::string_view &Name)
{
    if (const auto hash = StringViewHash{}(Name); m_namedShaders.contains(hash)) {
        return m_namedShaders.at(hash);
    }
    LOG_FORMAT("Shader '{}' does not exist!", Name)
    return {};
}

NonOwningPtr<Shader> ShaderManager::LoadShader(const std::vector<std::string_view> &Filepaths)
{
    const auto shader_name = File::remove_file_type_from_name(
        File::get_file_name_from_path(Filepaths[0]));
    const auto hash = StringViewHash{}(shader_name);
    if (m_namedShaders.contains(hash)) {
        LOG_FORMAT("Returned already loaded shader '{}'", shader_name)
        return m_namedShaders.at(hash);
    }

    std::vector<std::tuple<std::string_view, ShaderType>> shader_stages;
    for (auto &filepath : Filepaths) {
        auto file_ending = File::get_shader_type_from_spirv_path(filepath);
        if (file_ending == "vert")
            shader_stages.emplace_back(filepath, ShaderType::VERTEX);
        else if (file_ending == "frag")
            shader_stages.emplace_back(filepath, ShaderType::FRAGMENT);
    }
    const auto &new_shader = m_shaders.emplace_back(OwningPtr<Shader>(new Shader(shader_stages)));
    LOG_FORMAT("Loading shader '{}'", shader_name);
    return m_namedShaders.emplace(hash, NonOwningPtr<Shader>(new_shader)).first->second;
}

void ShaderManager::Shutdown()
{
    m_namedShaders.clear();
    m_shaders.clear();
}
}  // namespace Slipper
