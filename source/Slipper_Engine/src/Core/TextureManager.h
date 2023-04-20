#pragma once

namespace Slipper
{
class DepthBuffer;
class RenderTarget;
class Texture;
class Texture2D;

class TextureManager
{
 public:
    static NonOwningPtr<Texture2D> Load2D(std::string_view Filepath, bool GenerateMipMaps);
    static NonOwningPtr<Texture2D> Get2D(const std::string &Name);
    static void Shutdown();
    static std::map<std::string, NonOwningPtr<Texture>> &GetTextures();
    static const std::string &GetTextureName(NonOwningPtr<const Texture> Texture);

private:
    static inline std::vector<OwningPtr<Texture>> m_textures;
    static inline std::map<std::string, NonOwningPtr<Texture>> m_namedTextures;
};
}  // namespace Slipper