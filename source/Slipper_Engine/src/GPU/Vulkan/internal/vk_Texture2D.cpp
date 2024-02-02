#include "../vk_Texture2D.h"

#include "Vulkan/vk_Buffer.h"

namespace Slipper::GPU::Vulkan
{
Texture2D::Texture2D(const StbImage Image, const bool GenerateMipMaps)
    : Texture(vk::ImageType::e2D, Image.extent, Image.format, {}, GenerateMipMaps),
      filepath(Image.filepath)
{
    CreateTexture2D(Image.pixels);
    stbi_image_free(Image.pixels);
}

Texture2D::Texture2D(const VkExtent2D Extent,
                     const vk::Format ImageFormat,
                     std::optional<vk::Format> ViewFormat,
                     const bool GenerateMipMaps,
                     const vk::ImageTiling Tiling,
                     const vk::ImageUsageFlags Usage,
                     const vk::ImageAspectFlags ImageAspect,
                     const vk::MemoryPropertyFlags MemoryFlags)
    : Texture(vk::ImageType::e2D,
              VkExtent3D(Extent.width, Extent.height, 1),
              ImageFormat,
              ViewFormat,
              GenerateMipMaps,
              vk::SampleCountFlagBits::e1,
              Tiling,
              Usage,
              ImageAspect)
{
    CreateTexture2D(nullptr, MemoryFlags);
}

Texture2D::~Texture2D()
{
}

void Texture2D::CreateTexture2D(void *Data, const vk::MemoryPropertyFlags MemoryFlags)
{
    const VkDeviceSize texture_size = imageInfo.extent.width * imageInfo.extent.height * 4;

    const Buffer staging_buffer(texture_size,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                static_cast<VkMemoryPropertyFlags>(MemoryFlags));

    if (Data) {
        Buffer::SetBufferData(Data, staging_buffer);
        CopyBuffer(staging_buffer);
    }
}
}  // namespace Slipper