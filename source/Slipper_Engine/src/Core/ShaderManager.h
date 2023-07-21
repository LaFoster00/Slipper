#pragma once
#include "ComputeShader.h"
#include "GraphicsShader.h"

namespace Slipper
{

class ShaderManager
{
 public:
    static NonOwningPtr<Shader> GetShader(const std::string_view &Name);
    static NonOwningPtr<GraphicsShader> TryGetGraphicsShader(const std::string_view Name);
    static NonOwningPtr<ComputeShader> TryGetComputeShader(const std::string_view Name);
    static NonOwningPtr<GraphicsShader> LoadGraphicsShader(const std::vector<std::string_view> &Filepaths);
    static NonOwningPtr<ComputeShader> LoadComputeShader(const std::string_view Filepaths);
    static void Shutdown();

 private:
    static inline std::vector<OwningPtr<GraphicsShader>> m_graphicsShaders;
    static inline std::vector<OwningPtr<ComputeShader>> m_computeShaders;
    static inline std::map<size_t, NonOwningPtr<Shader>> m_namedShaders;
};
}  // namespace Slipper