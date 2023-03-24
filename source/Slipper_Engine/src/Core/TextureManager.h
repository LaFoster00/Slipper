#pragma once

namespace Slipper
{
class DepthBuffer;
class RenderTarget;
class Texture;
class Texture2D;

class TextureManager
{
    using OwningTextureVariants =
        std::variant<OwningPtr<Texture2D>, OwningPtr<RenderTarget>, OwningPtr<DepthBuffer>>;

    using NonOwningTextureVariants = std::
        variant<NonOwningPtr<Texture2D>, NonOwningPtr<RenderTarget>, NonOwningPtr<DepthBuffer>>;

 public:
    static NonOwningPtr<Texture2D> Load2D(std::string_view Filepath, bool GenerateMipMaps);
    static NonOwningPtr<Texture2D> Get2D(std::string_view Name);
    static void Shutdown();

 private:
    static std::vector<OwningTextureVariants> m_textures;
    static std::map<size_t, NonOwningTextureVariants> m_namedTextures;
};
}  // namespace Slipper