#pragma once

#include "Shader/Shader.h"

namespace Slipper
{
class ShaderManager
{
    using ShaderNameHash = std::hash<std::string_view>;
 public:
    static NonOwningPtr<Shader> GetShader(const std::string_view &Name);
    static NonOwningPtr<Shader> LoadShader(const std::vector<std::string_view> &Filepaths);
    static void Shutdown();

 private:
    static inline std::vector<OwningPtr<Shader>> m_shaders;
    static inline std::map<size_t, NonOwningPtr<Shader>> m_shader;
};
}  // namespace Slipper