#include "TextureManager.h"

#include "File.h"
#include "Path.h"
#include "Texture/Texture2D.h"

#include "Texture/DepthBuffer.h"

namespace Slipper
{
NonOwningPtr<Texture2D> TextureManager::Load2D(std::string_view Filepath, bool GenerateMipMaps)
{
    const auto texture_name = File::get_file_name_from_path(Filepath);
    if (m_namedTextures.contains(texture_name)) {
        if (m_namedTextures.at(texture_name)->GetImageInfo().type != vk::ImageType::e2D) {
            LOG_FORMAT(
                "Texture '{}' allready exists with another type. Rename the texture or load "
                "another one.",
                texture_name)
            return nullptr;
        }
        return m_namedTextures.at(texture_name)->As<Texture2D>();
    }

    VkExtent3D tex_dimensions{0, 0, 1};
    std::string absolute_path = Path::make_engine_relative_path_absolute(Filepath);
    stbi_uc *pixels = nullptr;
    {
        int tex_width, tex_height, tex_channels;
        pixels = stbi_load(
            absolute_path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

        tex_dimensions.width = tex_width;
        tex_dimensions.height = tex_height;
    }

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }

    const auto image = StbImage(
        pixels, tex_dimensions, Engine::TARGET_VIEWPORT_TEXTURE_FORMAT, std::move(absolute_path));

    const auto &new_texture =
        m_textures.emplace_back(new Texture2D(image, GenerateMipMaps))->As<Texture2D>();
    m_namedTextures.emplace(texture_name, new_texture);
    LOG_FORMAT("Loaded texture '{}' from {}", texture_name, Filepath);
    return new_texture;
}

NonOwningPtr<Texture2D> TextureManager::Get2D(const std::string &Name)
{
    if (m_namedTextures.contains(Name)) {
        if (m_namedTextures.at(Name)->GetImageInfo().type != vk::ImageType::e2D) {
            LOG_FORMAT("Texture '{}' is of other type than Texture2D. Returned nullptr", Name)
            return nullptr;
        }
        return m_namedTextures.at(Name)->As<Texture2D>();
    }
    LOG_FORMAT("Texture '{}' does not exist.", Name)
    return nullptr;
}

void TextureManager::Shutdown()
{
    m_namedTextures.clear();
    m_textures.clear();
}

std::map<std::string, NonOwningPtr<Texture>> &TextureManager::GetTextures()
{
    return m_namedTextures;
}

const std::string &TextureManager::GetTextureName(NonOwningPtr<const Texture> Texture)
{
    for (const auto &[name, texture] : m_namedTextures) {
        if (texture.get() == Texture.get()) {
            return name;
        }
    }
    throw std::invalid_argument(
        std::format("Texture '{}' is not valid!", std::bit_cast<size_t>(Texture.get())));
}
}  // namespace Slipper
