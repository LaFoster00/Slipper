#pragma once

#include "Engine.h"
#include "Texture.h"
#include <memory>
#include <string>

namespace Slipper
{
typedef unsigned char stbi_uc;

struct StbImage
{
    stbi_uc *pixels;
    VkExtent3D extent;
    VkFormat format;
    std::string filepath;
};

class Texture2D : public Texture
{
 public:
    ~Texture2D() override;
    Texture2D(StbImage Image, bool GenerateMipMaps);
    Texture2D(VkExtent2D Extent,
              VkFormat ImageFormat,
              std::optional<VkFormat> ViewFormat = {},
              bool GenerateMipMaps = true,
              VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL,
              VkImageUsageFlags Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT,
              VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
              VkMemoryPropertyFlags MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    static std::unique_ptr<Texture2D> LoadTexture(std::string_view Filepath,
                                                  bool GenerateMipMaps = true);

 private:
    void CreateTexture2D(void *Data,
                         VkMemoryPropertyFlags MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    Texture2D() = delete;

 public:
    std::string filepath;
};
}  // namespace Slipper