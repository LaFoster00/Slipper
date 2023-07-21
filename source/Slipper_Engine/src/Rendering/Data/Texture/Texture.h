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
            vk::Extent3D Extent,
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

    void AdditionalBindingChecks(const DescriptorSetLayoutBindingMinimal &Binding) const override
    {
    }

    virtual void Resize(const vk::Extent3D Extent);

    const std::vector<vk::ImageView> &GetViews() const
    {
        return imageInfo.views;
    }

    glm::vec3 GetSize()
    {
        return {imageInfo.extent.width, imageInfo.extent.height, imageInfo.extent.depth};
    }

    static void EnqueueTransitionImageLayout(vk::Image Image,
                                             ImageInfo &ImageInfo,
                                             vk::CommandBuffer CommandBuffer,
                                             vk::ImageLayout NewLayout);
    static SingleUseCommandBuffer CreateTransitionImageLayout(vk::Image Image,
                                                              ImageInfo &ImageInfo,
                                                              vk::ImageLayout NewLayout);

    void CopyBuffer(const Buffer &Buffer, bool TransitionToShaderUse = true);
    void EnqueueCopyImage(vk::CommandBuffer CommandBuffer,
                          vk::Image SrcImage,
                          vk::ImageLayout SrcLayout,
                          vk::Extent3D SrcExtent,
                          vk::ImageLayout TargetLayout);
    void CopyImage(vk::Image SrcImage,
                   vk::ImageLayout SrcLayout,
                   vk::Extent3D SrcExtent,
                   vk::ImageLayout TargetLayout);
    void EnqueueCopyTexture(vk::CommandBuffer CommandBuffer,
                            Texture &Texture,
                            vk::ImageLayout TargetLayout);
    void CopyTexture(Texture Texture, vk::ImageLayout TargetLayout);

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

    [[nodiscard]] static vk::ImageView CreateImageView(
        vk::Image Image,
        vk::ImageType Type,
        vk::Format Format,
        uint32_t MipLevels,
        vk::ImageAspectFlags ImageAspect = vk::ImageAspectFlagBits::eColor,
        uint32_t ArrayLayerCount = 1);

    [[nodiscard]] vk::ImageView CreateDefaultImageView() const;
    [[nodiscard]] vk::ImageView CreateImageView(vk::Format ViewFormat) const;

    static vk::Format FindSupportedFormat(const std::vector<vk::Format> &Candidates,
                                          vk::ImageTiling Tiling,
                                          vk::FormatFeatureFlags Features);
    static vk::Format FindDepthFormat();

    static bool HasStencilComponent(vk::Format Format);

 private:
    void Create();
    void EnqueueGenerateMipMaps(vk::CommandBuffer CommandBuffer);

 public:
    vk::Image vkImage;
    vk::DeviceMemory vkImageMemory;
    ImageInfo imageInfo;
    Sampler sampler;
};
}  // namespace Slipper