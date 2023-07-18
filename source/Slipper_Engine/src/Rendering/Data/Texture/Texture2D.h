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
              vk::ImageTiling Tiling = vk::ImageTiling::eOptimal,
              vk::ImageUsageFlags Usage = vk::ImageUsageFlagBits::eTransferDst |
                                          vk::ImageUsageFlagBits::eSampled,
              vk::ImageAspectFlags ImageAspect = vk::ImageAspectFlagBits::eColor,
              vk::MemoryPropertyFlags MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent);

 private:
    void CreateTexture2D(
        void *Data,
        vk::MemoryPropertyFlags MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible |
                                              vk::MemoryPropertyFlagBits::eHostCoherent);
    Texture2D() = delete;

 public:
    std::string filepath;
};
}  // namespace Slipper