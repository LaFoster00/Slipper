#include "Texture2D.h"

#include <stb_image.h>

#include "Buffer.h"
#include "Path.h"

Texture2D::Texture2D(const StbImage Image)
    : Texture(VK_IMAGE_TYPE_2D, Image.extent, Image.format), filepath(Image.filepath)
{
    const VkDeviceSize texture_size = Image.extent.width * Image.extent.height * 4;

    const Buffer staging_buffer(texture_size,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    Buffer::SetBufferData(Image.pixels, staging_buffer);
    stbi_image_free(Image.pixels);

    CopyBuffer(staging_buffer, *this);
}

Texture2D::~Texture2D()
{

}

std::unique_ptr<Texture2D> Texture2D::LoadTexture(const std::string_view Filepath)
{
    VkExtent3D tex_dimensions{0, 0, 1};
    const std::string absolute_path = Path::make_engine_relative_path_absolute(Filepath);
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

    auto image = StbImage(
        pixels, tex_dimensions, VK_FORMAT_R8G8B8A8_SRGB, std::move(absolute_path));
    return std::make_unique<Texture2D>(image);
}
