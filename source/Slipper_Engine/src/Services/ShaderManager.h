#pragma once

namespace Slipper
{
    namespace GPU::Vulkan
    {
        class ComputeShader;
        class GraphicsShader;
        class Shader;
    }

    class ShaderManager
    {
     public:
        static NonOwningPtr<GPU::Vulkan::Shader> GetShader(const std::string_view &Name);
        static NonOwningPtr<GPU::Vulkan::GraphicsShader> TryGetGraphicsShader(const std::string_view Name);
        static NonOwningPtr<GPU::Vulkan::ComputeShader> TryGetComputeShader(const std::string_view Name);
        static NonOwningPtr<GPU::Vulkan::GraphicsShader> LoadGraphicsShader(
            const std::vector<std::string_view> &Filepaths);
        static NonOwningPtr<GPU::Vulkan::ComputeShader> LoadComputeShader(const std::string_view Filepaths);
        static void Shutdown();

     private:
        static inline std::vector<OwningPtr<GPU::Vulkan::GraphicsShader>> m_graphicsShaders;
        static inline std::vector<OwningPtr<GPU::Vulkan::ComputeShader>> m_computeShaders;
        static inline std::map<size_t, NonOwningPtr<GPU::Vulkan::Shader>> m_namedShaders;
    };
}  // namespace Slipper
