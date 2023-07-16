#pragma once

#include "Drawing/CommandPool.h"
#include "Drawing/Sampler.h"
#include "IShaderBindableData.h"

namespace Slipper
{
class Texture;
class Sampler;
class Buffer;

struct ImageInfo
{
    vk::Sampler sampler = VK_NULL_HANDLE;
    std::vector<vk::ImageView> views;
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
    vk::ImageType type = vk::ImageType::e2D;
    vk::Extent3D extent = {0, 0, 0};
    vk::Format viewFormat = vk::Format::eUndefined;
    vk::Format imageFormat = vk::Format::eUndefined;
    vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
    vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment |
                                vk::ImageUsageFlagBits::eSampled;
    vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
    vk::ImageAspectFlags imageAspect = vk::ImageAspectFlagBits::eColor;
    uint32_t arrayLayerCount = 1;
    bool generateMipMaps = false;
    uint32_t mipLevels = 1;
};

class Texture : DeviceDependentObject, public IShaderBindableData
{
 public:
    Texture(vk::ImageType Type,
            VkExtent3D Extent,
            vk::Format ImageFormat,
            std::optional<vk::Format> ViewFormat = {},
            bool GenerateMipMaps = true,
            vk::SampleCountFlagBits NumSamples = vk::SampleCountFlagBits::e1,
            vk::ImageTiling Tiling = vk::ImageTiling::eOptimal,
            vk::ImageUsageFlags Usage = vk::ImageUsageFlagBits::eTransferDst |
                                        vk::ImageUsageFlagBits::eSampled,
            vk::ImageAspectFlags ImageAspect = vk::ImageAspectFlagBits::eColor,
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
        Other.imageInfo.views.clear();
    }

    virtual ~Texture();

    [[nodiscard]] std::optional<vk::DescriptorBufferInfo> GetDescriptorBufferInfo() const override
    {
        return {};
    }

    [[nodiscard]] std::optional<vk::DescriptorImageInfo> GetDescriptorImageInfo() const override
    {
        return vk::DescriptorImageInfo{
            imageInfo.sampler, imageInfo.views.front(), imageInfo.layout};
    }

    [[nodiscard]] constexpr vk::DescriptorType GetDescriptorType() const override
    {
        return vk::DescriptorType::eCombinedImageSampler;
    }

    void AdditionalBindingChecks(const DescriptorSetLayoutBinding &Binding) const override
    {
    }

    virtual void Resize(const VkExtent3D Extent);

    const std::vector<vk::ImageView> &GetViews() const
    {
        return imageInfo.views;
    }

    glm::vec3 GetSize()
    {
        return {imageInfo.extent.width, imageInfo.extent.height, imageInfo.extent.depth};
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

    const ImageInfo &GetImageInfo() const
    {
        return imageInfo;
    }

	vk::ImageLayout GetCurrentLayout() const
    {
        return imageInfo.layout;
    }

    operator vk::Image() const
    {
        return vkImage;
    }

    // Returns the texture as target type through dynamic_cast
    // Will return null if of other type
    template<typename T> NonOwningPtr<T> As()
    {
        return dynamic_cast<T *>(this);
    }

    [[nodiscard]] static VkImageView CreateImageView(
        VkImage Image,
        VkImageType Type,
        vk::Format Format,
        uint32_t MipLevels,
        VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT,
        uint32_t ArrayLayerCount = 1);

    [[nodiscard]] VkImageView CreateDefaultImageView() const;
    [[nodiscard]] VkImageView CreateImageView(vk::Format ViewFormat) const;

    static vk::Format FindSupportedFormat(const std::vector<vk::Format> &Candidates,
                                          VkImageTiling Tiling,
                                          VkFormatFeatureFlags Features);
    static vk::Format FindDepthFormat();

    static bool HasStencilComponent(vk::Format Format);

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