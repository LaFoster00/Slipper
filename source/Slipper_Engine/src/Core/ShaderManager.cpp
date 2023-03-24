#include "ShaderManager.h"

#include "File.h"

namespace Slipper
{
NonOwningPtr<Shader> ShaderManager::GetShader(const std::string_view &Name)
{
    if (const auto hash = ShaderNameHash{}(Name); m_shader.contains(hash)) {
        return m_shader.at(hash);
    }
    return {};
}

NonOwningPtr<Shader> ShaderManager::LoadShader(const std::vector<std::string_view> &Filepaths)
{
    const auto shader_name = File::get_file_name_from_path(Filepaths[0]);
    const auto hash = ShaderNameHash{}(shader_name);
    if (!m_shader.contains(hash)) {

        std::vector<std::tuple<std::string_view, ShaderType>> shader_stages;
        for (auto &filepath : Filepaths) {
            auto file_ending = File::get_file_ending_from_path(filepath);
            if (file_ending == "vert")
                shader_stages.emplace_back(filepath, ShaderType::VERTEX);
            else if (file_ending == "frag")
                shader_stages.emplace_back(filepath, ShaderType::FRAGMENT);
        }
        m_shaders.push_back(OwningPtr(new Shader(shader_name, shader_stages)));
        return m_shader.emplace(hash, NonOwningPtr(*m_shaders.end())).first->second;
    }
    return m_shader.at(hash);
}

void ShaderManager::Shutdown()
{
}
}  // namespace Slipper
