#include "Texture2D.h"

#include "Path.h"
#include "Buffer/Buffer.h"

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