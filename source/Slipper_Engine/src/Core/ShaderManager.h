#pragma once

#include "Shader.h"

namespace Slipper
{
class ShaderManager
{
 public:
    static NonOwningPtr<Shader> GetShader(const std::string_view &Name);
    static NonOwningPtr<Shader> LoadShader(const std::vector<std::string_view> &Filepaths);
    static void Shutdown();

 private:
    static inline std::vector<OwningPtr<Shader>> m_shaders;
    static inline std::map<size_t, NonOwningPtr<Shader>> m_namedShaders;
};
}  // namespace Slipper