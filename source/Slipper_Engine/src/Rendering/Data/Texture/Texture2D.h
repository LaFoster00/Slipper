#pragma once

#include "Texture.h"

namespace Slipper
{
typedef unsigned char stbi_uc;

struct StbImage
{
    stbi_uc *pixels;
    VkExtent3D extent;
    vk::Format format;
    std::string filepath;
};

class Texture2D : public Texture
{
 public:
    ~Texture2D() override;
    Texture2D(StbImage Image, bool GenerateMipMaps);
    Texture2D(VkExtent2D Extent,
              vk::Format ImageFormat,
              std::optional<vk::Format> ViewFormat = {},
              bool GenerateMipMaps = true,
              VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL,
              VkImageUsageFlags Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT,
              VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
              VkMemoryPropertyFlags MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

 private:
    void CreateTexture2D(void *Data,
                         VkMemoryPropertyFlags MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    Texture2D() = delete;

 public:
    std::string filepath;
};
}  // namespace Slipper