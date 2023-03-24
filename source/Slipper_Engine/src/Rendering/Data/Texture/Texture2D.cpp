#include "Texture2D.h"

#include "Buffer.h"
#include "Path.h"

namespace Slipper
{
Texture2D::Texture2D(const StbImage Image, const bool GenerateMipMaps)
    : Texture(VK_IMAGE_TYPE_2D, Image.extent, Image.format, {}, GenerateMipMaps),
      filepath(Image.filepath)
{
    CreateTexture2D(Image.pixels);
    stbi_image_free(Image.pixels);
}

Texture2D::Texture2D(const VkExtent2D Extent,
                     const VkFormat ImageFormat,
                     std::optional<VkFormat> ViewFormat,
                     const bool GenerateMipMaps,
                     const VkImageTiling Tiling,
                     const VkImageUsageFlags Usage,
                     const VkImageAspectFlags ImageAspect,
                     const VkMemoryPropertyFlags MemoryFlags)
    : Texture(VK_IMAGE_TYPE_2D,
              VkExtent3D(Extent.width, Extent.height, 1),
              ImageFormat,
              ViewFormat,
              GenerateMipMaps,
              VK_SAMPLE_COUNT_1_BIT,
              Tiling,
              Usage,
              ImageAspect)
{
    CreateTexture2D(nullptr, MemoryFlags);
}

Texture2D::~Texture2D()
{
}

OwningPtr<Texture2D> Texture2D::LoadTexture(const std::string_view Filepath,
                                                  const bool GenerateMipMaps)
{
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

    auto image = StbImage(
        pixels, tex_dimensions, Engine::TARGET_VIEWPORT_TEXTURE_FORMAT, std::move(absolute_path));
    return new Texture2D(image, GenerateMipMaps);
}

void Texture2D::CreateTexture2D(void *Data, const VkMemoryPropertyFlags MemoryFlags)
{
    const VkDeviceSize texture_size = imageInfo.extent.width * imageInfo.extent.height * 4;

    const Buffer staging_buffer(texture_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemoryFlags);

    if (Data) {
        Buffer::SetBufferData(Data, staging_buffer);
        CopyBuffer(staging_buffer);
    }
}
}  // namespace Slipper