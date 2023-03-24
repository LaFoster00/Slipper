#pragma once

namespace Slipper
{
class Texture2D;

class TextureManager
{
 public:
    static NonOwningPtr<Texture2D> Load2D(std::string_view Filepath, bool GenerateMipMaps);
    static NonOwningPtr<Texture2D> Get2D(std::string_view Name);
    static void Shutdown();
};
}  // namespace Slipper