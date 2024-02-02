#pragma once

namespace Slipper
{
    namespace GPU::Vulkan
    {
        class Texture;
        class Texture2D;
    }  // namespace GPU::Vulkan

    class TextureManager
    {
     public:
        static NonOwningPtr<GPU::Vulkan::Texture2D> Load2D(std::string_view Filepath, bool GenerateMipMaps);
        static NonOwningPtr<GPU::Vulkan::Texture2D> Get2D(const std::string &Name);
        static void Shutdown();
        static std::map<std::string, NonOwningPtr<GPU::Vulkan::Texture>> &GetTextures();
        static const std::string &GetTextureName(NonOwningPtr<const GPU::Vulkan::Texture> Texture);

     private:
        static inline std::vector<OwningPtr<GPU::Vulkan::Texture>> m_textures;
        static inline std::map<std::string, NonOwningPtr<GPU::Vulkan::Texture>> m_namedTextures;
    };
}  // namespace Slipper
