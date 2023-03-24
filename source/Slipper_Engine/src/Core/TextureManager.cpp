#include "TextureManager.h"

#include "File.h"
#include "Texture/DepthBuffer.h"
#include "Texture/RenderTarget.h"
#include "Texture/Texture2D.h"

namespace Slipper
{
std::vector<TextureManager::OwningTextureVariants> TextureManager::m_textures;
std::map<size_t, TextureManager::NonOwningTextureVariants> TextureManager::m_namedTextures;

NonOwningPtr<Texture2D> TextureManager::Load2D(std::string_view Filepath, bool GenerateMipMaps)
{
    const auto texture_name = File::get_file_name_from_path(Filepath);
    const auto texture_name_hash = StringViewHash{}(texture_name);
    if (m_namedTextures.contains(texture_name_hash)) {
        if (!std::holds_alternative<NonOwningPtr<Texture2D>>(
                m_namedTextures.at(texture_name_hash))) {
            LOG_FORMAT(
                "Texture '{}' allready exists with another type. Rename the texture or load "
                "another one.",
                texture_name)
            return nullptr;
        }
        return std::get<NonOwningPtr<Texture2D>>(m_namedTextures.at(texture_name_hash));
    }
    const auto &new_texture = std::get<OwningPtr<Texture2D>>(
        m_textures.emplace_back(Texture2D::LoadTexture(Filepath, GenerateMipMaps)));
    m_namedTextures.emplace(texture_name_hash, NonOwningPtr<Texture2D>(new_texture));
    LOG_FORMAT("Loaded texture '{}' from {}", texture_name, Filepath);
    return new_texture;
}

NonOwningPtr<Texture2D> TextureManager::Get2D(std::string_view Name)
{
    const auto texture_name_hash = StringViewHash{}(Name);
    if (m_namedTextures.contains(texture_name_hash)) {
        if (!std::holds_alternative<NonOwningPtr<Texture2D>>(
                m_namedTextures.at(texture_name_hash))) {
            LOG_FORMAT("Texture '{}' is of other type than Texture2D. Returned nullptr", Name)
            return nullptr;
        }
        return std::get<NonOwningPtr<Texture2D>>(m_namedTextures.at(texture_name_hash));
    }
    LOG_FORMAT("Texture '{}' does not exist.", Name)
    return nullptr;
}

void TextureManager::Shutdown()
{
    m_namedTextures.clear();
    m_textures.clear();
}
}  // namespace Slipper
