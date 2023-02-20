#pragma once

#include "Drawing/CommandPool.h"
#include "Drawing/Sampler.h"

namespace Slipper
{
class Sampler;
class Buffer;

struct ImageInfo
{
    VkSampler sampler = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageType type = VK_IMAGE_TYPE_2D;
    VkExtent3D extent = {0, 0, 0};
    VkFormat viewFormat = VK_FORMAT_UNDEFINED;
    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
    uint32_t arrayLayerCount = 1;
    bool generateMipMaps = false;
    uint32_t mipLevels = 1;
};

class Texture : DeviceDependentObject
{
 public:
    Texture(VkImageType Type,
            VkExtent3D Extent,
            VkFormat ImageFormat,
            std::optional<VkFormat> ViewFormat = {},
            bool GenerateMipMaps = true,
            VkSampleCountFlagBits NumSamples = VK_SAMPLE_COUNT_1_BIT,
            VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlags Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
            uint32_t ArrayLayers = 1);

    Texture(const Texture &Other) = delete;

    Texture(Texture &&Other) noexcept
        : vkImage(Other.vkImage),
          vkImageMemory(Other.vkImageMemory),
          imageInfo(std::move(Other.imageInfo)),
          sampler(std::move(Other.sampler))
    {
        Other.vkImage = VK_NULL_HANDLE;
        Other.vkImageMemory = VK_NULL_HANDLE;
        Other.imageInfo.view = VK_NULL_HANDLE;
    }

    virtual ~Texture();

    virtual void Resize(const VkExtent3D Extent);
    VkDescriptorImageInfo GetDescriptorImageInfo() const
    {
        return VkDescriptorImageInfo{imageInfo.sampler, imageInfo.view, imageInfo.layout};
    }

    static void EnqueueTransitionImageLayout(VkImage Image,
                                             ImageInfo &ImageInfo,
                                             VkCommandBuffer CommandBuffer,
                                             VkImageLayout NewLayout);
    static SingleUseCommandBuffer CreateTransitionImageLayout(VkImage Image,
                                                              ImageInfo &ImageInfo,
                                                              VkImageLayout NewLayout);

    void CopyBuffer(const Buffer &Buffer, bool TransitionToShaderUse = true);
    void EnqueueCopyImage(VkCommandBuffer CommandBuffer,
                          VkImage SrcImage,
                          VkImageLayout SrcLayout,
                          VkExtent3D SrcExtent,
                          VkImageLayout TargetLayout);
    void CopyImage(VkImage SrcImage,
                   VkImageLayout SrcLayout,
                   VkExtent3D SrcExtent,
                   VkImageLayout TargetLayout);
    void EnqueueCopyTexture(VkCommandBuffer CommandBuffer,
                            Texture &Texture,
                            VkImageLayout TargetLayout);
    void CopyTexture(Texture Texture, VkImageLayout TargetLayout);

    const ImageInfo GetImageInfo() const
    {
        return imageInfo;
    }

    const VkImageLayout GetCurrentLayout() const
    {
        return imageInfo.layout;
    }

    operator VkImage() const
    {
        return vkImage;
    }

    operator VkImageView() const
    {
        return imageInfo.view;
    }

    [[nodiscard]] static VkImageView CreateImageView(
        VkImage Image,
        VkImageType Type,
        VkFormat Format,
        uint32_t MipLevels,
        VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
        uint32_t ArrayLayerCount = 1);

    static VkFormat FindSupportedFormat(const std::vector<VkFormat> &Candidates,
                                        VkImageTiling Tiling,
                                        VkFormatFeatureFlags Features);
    static VkFormat FindDepthFormat();

    static bool HasStencilComponent(const VkFormat Format);

 private:
    void Create();
    void EnqueueGenerateMipMaps(VkCommandBuffer CommandBuffer);

 public:
    VkImage vkImage;
    VkDeviceMemory vkImageMemory;
    ImageInfo imageInfo;
    Sampler sampler;
};
}  // namespace Slipper