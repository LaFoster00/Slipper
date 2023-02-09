#pragma once

#include <memory>

#include "DeviceDependentObject.h"
#include "Drawing/CommandPool.h"
#include "Drawing/Sampler.h"

namespace Slipper
{
class Sampler;
class Buffer;

class Texture : DeviceDependentObject
{
 public:
    Texture(VkImageType Type,
            VkExtent3D Extent,
            VkFormat Format,
            bool GenerateMipMaps = true,
            VkSampleCountFlagBits NumSamples = VK_SAMPLE_COUNT_1_BIT,
            VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlags Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
            uint32_t ArrayLayers = 1);

    Texture(const Texture &Other) = delete;

    Texture(Texture &&Other) noexcept
        : generateMipMaps(Other.generateMipMaps),
          mipLevels(Other.mipLevels),
          texture(Other.texture),
          textureMemory(Other.textureMemory),
          sampler(std::move(Other.sampler)),
          textureView(Other.textureView),
          type(Other.type),
          extent(Other.extent),
          format(Other.format),
          tiling(Other.tiling),
          usage(Other.usage),
          numSamples(Other.numSamples),
          imageAspect(0),
          arrayLayerCount(Other.arrayLayerCount),
          imageInfo(std::move(Other.imageInfo))
    {
        Other.texture = VK_NULL_HANDLE;
        Other.textureMemory = VK_NULL_HANDLE;
        Other.textureView = VK_NULL_HANDLE;
    }

    virtual ~Texture();

    virtual void Resize(const VkExtent3D Extent);

    void EnqueueTransitionImageLayout(VkCommandBuffer CommandBuffer, VkImageLayout NewLayout);
    SingleUseCommandBuffer CreateTransitionImageLayout(VkImageLayout NewLayout);
    void CopyBuffer(const Buffer &Buffer, bool TransitionToShaderUse = true);
    const VkDescriptorImageInfo *GetDescriptorImageInfo() const;

    operator VkImage() const
    {
        return texture;
    }

    operator VkImageView() const
    {
        return textureView;
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
    bool generateMipMaps;
    uint32_t mipLevels;

    VkImage texture;
    VkDeviceMemory textureMemory;
    Sampler sampler;

    VkImageView textureView;

    VkImageType type;
    VkExtent3D extent;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkSampleCountFlagBits numSamples;

    VkImageAspectFlags imageAspect;
    uint32_t arrayLayerCount;

    VkDescriptorImageInfo imageInfo;
};
}